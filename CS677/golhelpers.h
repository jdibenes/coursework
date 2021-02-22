
#ifndef GOLHELPERS_H
#define GOLHELPERS_H

#include "goltypes.h"

namespace gol {
struct header {
    u32 width;
    u32 height;
};

void loadRandom(gol::cell *data, int width, int height);
bool saveFile(char const *filename, gol::cell const *data, int width, int height);
bool loadHeader(char const *filename, int &width, int &height);
bool loadFile(char const *filename, gol::cell *data, int width, int height);
void sanitize(gol::cell *data, int width, int height);

gol::cell fetchDTD(gol::cell *current, int width, int height, int x, int y);
gol::cell fetchDTA(gol::cell *current, int width, int height, int x, int y);
gol::cell fetchWrap(gol::cell *current, int width, int height, int x, int y);
}

#endif // GOLHELPERS_H
