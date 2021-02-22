
#ifndef UTILITIES_H
#define UTILITIES_H

namespace utilities {
typedef void (*memcpy_function)(void *, void *, int);

int roundPOT(int value);
void tile(int width, int height, int TILE_L, int &rows, int &cols, int &tiles);
void divPOTshrmask(int pot, int &shr, int &mask);
void memcpywrap(void *dst, void *src, int n);
void memcpyswap(void *dst, void *src, int n);
void reshapeArray2DTexture3D(int width, int height, int TILE_L, char *tex, char *arr, bool swap);
}

#endif // UTILITIES_H
