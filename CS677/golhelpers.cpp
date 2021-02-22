
#include <stdlib.h>
#include <stdio.h>
#include "golgrid.h"
#include "golhelpers.h"

namespace gol {
void loadRandom(gol::cell *data, int width, int height) {
    for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width;  ++col) { data[(row * width) + col] = (rand() & 1) - 1; }
    }
}

bool saveFile(char const *filename, const gol::cell *data, int width, int height) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) { return false; }

    gol::header h;

    h.width  = width;
    h.height = height;

    int bytesh = sizeof(h);
    int bytesg = width * height * sizeof(gol::cell);

    int bytes = fwrite(&h, 1, bytesh, f) + fwrite(data, 1, bytesg, f);

    fclose(f);

    return bytes == (bytesh + bytesg);
}

bool loadHeader(char const *filename, int &width, int &height) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) { return false; }

    gol::header h;

    int bytesh = fread(&h, 1, sizeof(h), f);

    fclose(f);

    if (bytesh != sizeof(h)) { return false; }

    u64 bytesgl = (u64)h.width * (u64)h.height;
    if (bytesgl >= GOL_MAX_CELLS) { return false; }

    width  = h.width;
    height = h.height;

    return true;
}

bool loadFile(char const *filename, gol::cell *data, int width, int height) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) { return false; }

    fseek(f, sizeof(gol::header), SEEK_SET);

    int bytesg = width * height * sizeof(gol::cell);
    int bytes  = fread(data, 1, bytesg, f);

    fclose(f);

    return bytes == bytesg;
}

void sanitize(gol::cell *data, int width, int height) {
    int elements = width * height;
    for (int k = 0; k < elements; ++k) { data[k] = data[k] ? GOL_STATE_DEAD : GOL_STATE_ALIVE; }
}

gol::cell fetchDTD(gol::cell *current, int width, int height, int x, int y) {
    return (x >= 0 && x < width && y >= 0 && y < height) ? current[(y * width) + x] : GOL_STATE_DEAD;
}

gol::cell fetchDTA(gol::cell *current, int width, int height, int x, int y) {
    return (x >= 0 && x < width && y >= 0 && y < height) ? current[(y * width) + x] : GOL_STATE_ALIVE;
}

gol::cell fetchWrap(gol::cell *current, int width, int height, int x, int y) {
    if (x < 0) { x += width;  } else if (x >= width)  { x -= width;  }
    if (y < 0) { y += height; } else if (y >= height) { y -= height; }

    return current[(y * width) + x];
}
}
