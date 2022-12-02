#pragma once

#include <lore/lore.h>
#include <lore/lens/Surface.h>

#ifdef __METAL__
namespace metal {

template<typename T>
class vector {
public:
    struct const_iterator {
        const_iterator(device const T *data) : data(data) {}
        
        device const T &operator*() const { return *data; }
        device const T *operator->() const { return data; }
        
        thread const_iterator &operator++() { data++; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; data++; return tmp; }
        
        friend bool operator==(const_iterator a, const_iterator b) { return a.data == b.data; }
        friend bool operator!=(const_iterator a, const_iterator b) { return a.data != b.data; }
    private:
        device const T *data;
    };
    
    const_iterator begin() const {
        return const_iterator(m_data+0);
    }
    
    const_iterator end() const {
        return const_iterator(m_data+m_size);
    }
    
    device T &operator[](int i) {
        return m_data[i];
    }
    
    device const T &operator[](int i) const {
        return m_data[i];
    }
    
    int size() const {
        return m_size;
    }
    
    int m_size;
    device T *m_data;
};

}
#else
#include <vector>
#endif

namespace lore {

template<typename Float = float>
struct Lens {
    std::vector<Surface<Float>> surfaces;
};

}
