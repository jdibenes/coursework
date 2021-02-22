
#include <strings.h>
#include <string.h>
#include "utilities.h"

namespace utilities {
int roundPOT(int value) {
    int msb = 1U << (31 - __builtin_clz(value));
    return ((value & (msb - 1)) != 0) ? (msb << 1) : msb;
}

void tile(int width, int height, int TILE_L, int &rows, int &cols, int &tiles) {
    rows = (height / TILE_L) + ((height % TILE_L) != 0);
    cols = (width  / TILE_L) + ((width  % TILE_L) != 0);

    tiles = rows * cols;
}

void divPOTshrmask(int pot, int &shr, int &mask) {
    shr  = ffs(pot) - 1;
    mask = pot - 1;
}

void memcpywrap(void *dst, void *src, int n) {
    memcpy(dst, src, n);
}

void memcpyswap(void *dst, void *src, int n) {
    memcpy(src, dst, n);
}

void reshapeArray2DTexture3D(int width, int height, int TILE_L, char *tex, char *arr, bool swap) {
    int shr;
    int mask;

    divPOTshrmask(TILE_L, shr, mask);

    memcpy_function copy = swap ? &memcpyswap : &memcpywrap;

    int wdv = width >> shr;
    int wrm = width & mask;

    int tex_hstride = TILE_L * TILE_L;
    int tex_vstride = (wdv + (wrm != 0)) * tex_hstride;

    for (int row = 0; row < height; ++row) {
        int tex_voffset = (row >> shr) * tex_vstride + (row & mask) * TILE_L;
        int arr_voffset = row * width;

    for (int col = 0; col < wdv; ++col) {
        int tex_hoffset = col * tex_hstride;
        int arr_hoffset = col * TILE_L;

        copy(tex + tex_voffset + tex_hoffset,       arr + arr_voffset + arr_hoffset,  TILE_L);
    }
    if (wrm) {
        copy(tex + tex_voffset + wdv * tex_hstride, arr + arr_voffset + wdv * TILE_L, wrm);
    }
    }
}
}
