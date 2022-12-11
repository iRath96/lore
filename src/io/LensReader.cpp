#include <lore/io/LensReader.h>
#include <lore/lens/GlassCatalog.h>
#include <lore/logging.h>

#include <iostream>
#include <sstream>
#include <exception>

namespace lore {
namespace io {

struct Position {
    int line = 1;
    int column = 1;
};

struct Token {
    enum Type {
        KEYWORD,
        STRING,
        NUMBER,
        COMMENT,
        NONE
    };

    static std::string typeToString(Type type) {
        switch (type) {
            case KEYWORD:
                return "keyword";
            case STRING:
                return "string";
            case NUMBER:
                return "number";
            case COMMENT:
                return "comment";
            case NONE:
                return "none";
        }
        return "(invalid)";
    }

    Type type;
    Position pos;
    std::string text;
};

class TokenizerException : public std::exception {
public:
    TokenizerException(const std::string &msg, const Position &pos) {
        std::stringstream stream;
        stream << msg << " at " << pos.line << ":" << pos.column;
        m_msg = stream.str();
    }

    virtual const char *what() const throw() {
        return m_msg.c_str();
    }

private:
    std::string m_msg;
};

class IStreamPosition {
public:
    IStreamPosition(std::istream &is) : m_is(is) {}

    int peek() { return m_is.peek(); }

    char get() {
        const char chr = m_is.get();
        if (chr == '\n') {
            m_pos.line++;
            m_pos.column = 1;
        } else {
            m_pos.column++;
        }
        return chr;
    }

    Position pos() const {
        return m_pos;
    }

private:
    std::istream &m_is;
    Position m_pos;
};

class Tokenizer {
public:
    Tokenizer(std::istream &is) : m_stream(is) {}

    bool next(Token &token) {
        token.type = peek();
        token.pos = m_pos;

        switch (token.type) {
            case Token::NONE:
                return false;

            case Token::STRING:
                token.text = readString();
                return true;

            case Token::COMMENT:
                token.text = readComment();
                return true;

            default:
                token.text = readGeneric();
                return true;
        }
    }

    Token::Type peek() {
        // skip whitespace
        while (isWhitespace(m_stream.peek()))
            m_stream.get();

        const char next = m_stream.peek();
        if (next == EOF) {
            return Token::NONE;
        } else if (next == '"') {
            return Token::STRING;
        } else if (next == '/') {
            return Token::COMMENT;
        } else if (isNumeric(next)) {
            return Token::NUMBER;
        } else {
            return Token::KEYWORD;
        }
    }

    Token expect(Token::Type type) {
        Token result;
        while (true) {
            if (!next(result)) {
                error(
                    "Expected " + Token::typeToString(type) +
                    ", but found end of stream"
                );
            }

            // skip comments
            if (result.type != Token::COMMENT) {
                break;
            }
        }

        if (result.type != type) {
            error(
                "Expected " + Token::typeToString(type) +
                ", but got " + Token::typeToString(result.type) +
                " '" + result.text + "'"
            );
        }

        return result;
    }

    void expectKeyword(const std::string &text) {
        Token result = expect(Token::KEYWORD);
        if (result.text != text) {
            error(
                "Expected '" + text + "' keyword but found '" + result.text + "' instead"
            );
        }
    }

    std::string expectString() {
        return expect(Token::STRING).text;
    }

    int expectInt() {
        return std::stoi(expect(Token::NUMBER).text);
    }

    float expectFloat() {
        return std::stof(expect(Token::NUMBER).text);
    }

private:
    static bool isWhitespace(char chr) {
        return chr == '\t' || chr == '\r' || chr == '\n' || chr == ' ';
    }

    static bool isNumeric(char chr) {
        return (chr >= '0' && chr <= '9') ||
               (chr == '.' || chr == '+' || chr == '-');
    }

    static bool isAlphaNumeric(char chr) {
        return
            (chr >= 'a' && chr <= 'z') ||
            (chr >= 'A' && chr <= 'Z') ||
            isNumeric(chr);
    }

    static bool isGenericChar(char chr) {
        return isAlphaNumeric(chr) || (chr == '_' || chr == '-');
    }

    std::string readGeneric() {
        if (!isAlphaNumeric(m_stream.peek())) {
            error("Unexpected character");
        }

        std::string result = "";
        while (true) {
            if (!isGenericChar(m_stream.peek())) {
                break;
            }
            result += m_stream.get();
        }
        return result;
    }

    std::string readComment() {
        for (int i = 0; i < 2; i++) {
            if (!(m_stream.get() == '/')) {
                error("Expected comment");
            }
        }

        std::string result = "";
        while (true) {
            const char next = m_stream.get();
            if (next == '\n' || next == EOF) {
                break;
            }
            result += next;
        }

        return result;
    }

    std::string readString() {
        if (!(m_stream.get() == '"')) {
            error("Expected string");
        }

        std::string result = "";
        while (true) {
            const char next = m_stream.get();
            if (next == '"') {
                break;
            } else if (next == '\\') {
                const char esc = m_stream.get();
                switch (esc) {
                    case 'n':
                        result += "\n";
                        break;
                    default:
                        result += esc;
                        break;
                }
            } else if (next == EOF) {
                error("Unterminated string");
            } else {
                result += next;
            }
        }
        return result;
    }

    void error(const std::string &msg) {
        throw TokenizerException(msg, m_stream.pos());
    }

    IStreamPosition m_stream;
    Position m_pos;
};

struct ParserResult {
    std::vector<LensSchema<float>> lenses;
};

class Parser {
public:
    const GlassCatalog &glassCatalog;

    Parser(const GlassCatalog &glassCatalog) : glassCatalog(glassCatalog) {}

    ParserResult parse(std::istream &is) const {
        Tokenizer tokenizer(is);
        ParserResult result;

        while (true) {
            Token token;
            if (!tokenizer.next(token)) {
                break;
            }

            if (token.type == Token::COMMENT) {
                log::debug() << token.text << std::flush;
                continue;
            } else if (token.type != Token::KEYWORD) {
                log::warning() << "unexpected " << Token::typeToString(token.type) << std::flush;
                continue;
            }

            // handle keywords
            if (token.text == "LEN") {
                result.lenses.push_back(parseLens(tokenizer));
            } else if (token.text == "DLRS") {
                // @todo ??
                tokenizer.expectInt();
            } else if (token.text == "DLNR") {
                // @todo ??
                tokenizer.expectInt();
                tokenizer.expectInt();
            } else if (token.text == "DLMN") {
                // @todo ??
                tokenizer.expectInt();
                tokenizer.expectFloat();
            } else if (token.text == "DLMX") {
                // @todo ??
                tokenizer.expectInt();
                tokenizer.expectFloat();
            } else if (token.text == "DLAS") {
                // @todo ??
                tokenizer.expect(Token::KEYWORD);
            } else {
                log::warning() << "unknown keyword '" << token.text << "'" << std::flush;
            }
        }

        return result;
    }

private:
    SurfaceSchema<float> defaultSurface() const {
        SurfaceSchema<float> surface;
        surface.checkAperture = false;
        surface.radius = 0;
        surface.aperture = 0;
        surface.thickness = 0;
        surface.glass = Glass<float>::air();
        return surface;
    }

    LensSchema<float> parseLens(Tokenizer &tokenizer) const {
        LensSchema<float> lens;
        lens.stopIndex = 1;

        tokenizer.expectKeyword("NEW");
        lens.name = tokenizer.expectString();
        tokenizer.expectFloat(); // EFL
        tokenizer.expectInt(); // num surfaces

        SurfaceSchema<float> surface = defaultSurface();
        while (true) {
            const Token token = tokenizer.expect(Token::KEYWORD);

            // lens commands
            if (token.text == "EBR") {
                lens.entranceBeamRadius = tokenizer.expectFloat();
            } else if (token.text == "ANG") {
                lens.fieldAngle = tokenizer.expectFloat();
            } else if (token.text == "DES") {
                lens.description = tokenizer.expectString();
            } else if (token.text == "UNI") {
                // @todo ??
                tokenizer.expectFloat();
            } else

            // surface commands
            if (token.text == "AIR") {
                surface.glassName = "AIR";
                surface.glass = Glass<float>::air();
            } else if (token.text == "GLA") {
                const std::string name = tokenizer.expect(Token::KEYWORD).text;
                surface.glassName = name;
                surface.glass = glassCatalog.glass(name);
            } else if (token.text == "RD") {
                surface.radius = tokenizer.expectFloat();
            } else if (token.text == "TH") {
                surface.thickness = tokenizer.expectFloat();
            } else if (token.text == "AP") {
                if (tokenizer.peek() == Token::KEYWORD) {
                    tokenizer.expectKeyword("CHK");
                    surface.checkAperture = true;
                } else {
                    surface.checkAperture = false;
                }
                surface.aperture = tokenizer.expectFloat();
            } else if (token.text == "AST") {
                lens.stopIndex = int(lens.surfaces.size());
            } else if (token.text == "DRW") {
                // @todo
                tokenizer.expect(Token::KEYWORD);
            } else if (token.text == "CBK") {
                tokenizer.expectInt();
            } else

            // wavelength commands
            if (token.text == "WV") {
                while (tokenizer.peek() == Token::NUMBER) {
                    WeightedWavelength<float> ww;
                    ww.wavelength = tokenizer.expectFloat();
                    ww.weight = 1;
                    lens.wavelengths.push_back(ww);
                }
            } else if (token.text == "WW") {
                int i = 0;
                while (tokenizer.peek() == Token::NUMBER) {
                    if (i >= lens.wavelengths.size()) {
                        log::warning() << "too many wavelength weights given" << std::flush;
                        break;
                    }
                    lens.wavelengths[i++].weight = tokenizer.expectFloat();
                }
            } else

            // flow commands
            if (token.text == "NXT") {
                lens.surfaces.push_back(surface);
                surface = defaultSurface();
            } else if (token.text == "END") {
                tokenizer.expectInt();
                lens.surfaces.push_back(surface);
                surface = defaultSurface();
                break;
            } else

            {
                log::warning() << "unknown surface command '" << token.text << "'" << std::flush;
            }
        }

        return lens;
    }
};

std::vector<LensSchema<float>> LensReader::read(std::istream &is) const {
    Parser parser{glassCatalog};
    ParserResult result = parser.parse(is);
    return result.lenses;
}

}
}
