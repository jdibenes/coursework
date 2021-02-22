
#ifndef GOLGRID_H
#define GOLGRID_H

#include "goltypes.h"

namespace gol {
typedef gol::cell (*fetch)(gol::cell *current, int width, int height, int x, int y);

class grid {
private:
    gol::cell *m_current;
    gol::cell *m_next;

    int m_width;
    int m_height;
    int m_cells;
    int m_bytes;

    gol::fetch m_fetch;

    void swapBuffers();

public:
    grid();
    ~grid();

    void create(int width, int height);
    void destroy() const;
    void setFetch(gol::fetch fetch);

    void read(gol::cell *data) const;
    gol::cell read(int x, int y) const;
    void write(gol::cell const *data) const;
    void write(int x, int y, gol::cell data) const;
    gol::cell fetch(int x, int y) const;
    int aliveNeighbors(int x, int y) const;
    gol::cell nextState(int x, int y) const;
    void singleStep();

    gol::cell *current() const;
    gol::cell *next() const;
    int width() const;
    int height() const;
    int cells() const;
    int bytes() const;
    gol::fetch fetch() const;
};
}

#endif // GOLGRID_H
