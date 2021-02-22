
#ifndef CUDAGOLGRID_H
#define CUDAGOLGRID_H

#include <cuda_runtime.h>
#include "goltypes.h"

namespace cudagol {
class grid {
private:
    gol::cell *m_d_current;
    gol::cell *m_d_next;

    int m_width;
    int m_height;
    int m_bytes;

    dim3 m_grid;
    dim3 m_block;

    void swapBuffers();

public:
    grid();
    ~grid();

    void create(int width, int height);
    void destroy();

    void read(gol::cell *data) const;
    void write(gol::cell const *data) const;
    void singleStep();

    gol::cell *d_current() const;
    gol::cell *d_next() const;
    int width() const;
    int height() const;
    int bytes() const;
};
}

#endif // CUDAGOLGRID_H
