
#include <string.h>
#include "golgrid.h"

namespace gol {
grid::grid() {
}

grid::~grid() {
}

void grid::create(int width, int height) {
    m_width  = width;
    m_height = height;
    m_cells  = width * height;
    m_bytes  = m_cells * sizeof(gol::cell);

    m_current = new gol::cell[m_cells];
    m_next    = new gol::cell[m_cells];
}

void grid::destroy() const {
    delete [] m_next;
    delete [] m_current;
}

void grid::setFetch(gol::fetch function) {
    m_fetch = function;
}

void grid::read(gol::cell *data) const {
    memcpy(data, m_current, m_bytes);
}

gol::cell grid::read(int x, int y) const {
    return m_current[(y * m_width) + x];
}

void grid::write(gol::cell const *data) const {
    memcpy(m_current, data, m_bytes);
}

void grid::write(int x, int y, gol::cell data) const {
    m_current[(y * m_width) + x] = data;
}

gol::cell grid::fetch(int x, int y) const {
    return m_fetch(m_current, m_width, m_height, x, y);
}

int grid::aliveNeighbors(int x, int y) const {
    return fetch(x - 1, y - 1) + fetch(x, y - 1) + fetch(x + 1, y - 1) +
           fetch(x - 1, y    ) + 8               + fetch(x + 1, y    ) +
           fetch(x - 1, y + 1) + fetch(x, y + 1) + fetch(x + 1, y + 1);
}

gol::cell grid::nextState(int x, int y) const {
    switch (aliveNeighbors(x, y)) {
    case 2:  return fetch(x, y);
    case 3:  return GOL_STATE_ALIVE;
    default: return GOL_STATE_DEAD;
    }
}

void grid::singleStep() {
    for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) { m_next[(y * m_width) + x] = nextState(x, y); }
    }

    swapBuffers();
}

void grid::swapBuffers() {
    gol::cell *tmp = m_current;
    m_current = m_next;
    m_next = tmp;
}

gol::cell *grid::current() const {
    return m_current;
}

gol::cell *grid::next() const {
    return m_next;
}

int grid::width() const{
    return m_width;
}

int grid::height() const{
    return m_height;
}

int grid::cells() const{
    return m_cells;
}

int grid::bytes() const{
    return m_bytes;
}

gol::fetch grid::fetch() const {
    return m_fetch;
}
}
