#pragma once

#ifdef __METAL__
#include <metal_stdlib>
#define std metal
#define MTL_THREAD thread
#define MTL_DEVICE device
#else
#define MTL_THREAD
#define MTL_DEVICE
#endif
