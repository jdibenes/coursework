
#include "cudagolgrid.h"

#define BLOCK_WIDTH  32
#define BLOCK_HEIGHT 16

namespace {
template <int MASK_WIDTH, int MASK_HEIGHT>
__global__ void kernelGOLNaive(gol::cell const *current, gol::cell *next, int width, int height) {
    __shared__ gol::cell tile[MASK_HEIGHT][MASK_WIDTH];

    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int bx = MASK_WIDTH  - 2;
    int by = MASK_HEIGHT - 2;
    int x  = (blockIdx.x * bx) + tx;
    int y  = (blockIdx.y * by) + ty;

    int lx;
    int ly;

    if (x < 1) { lx = x - 1 + width;  } else if (x <= width ) { lx = x - 1; } else { lx = x - 1 - width;  }
    if (y < 1) { ly = y - 1 + height; } else if (y <= height) { ly = y - 1; } else { ly = y - 1 - height; }

    tile[ty][tx] = current[(ly * width) + lx];

    __syncthreads();

    if ((ty >= by) || (tx >= bx)) { return; }

    gol::cell alive = tile[ty    ][tx] + tile[ty    ][tx + 1] + tile[ty    ][tx + 2] +
                      tile[ty + 1][tx] + 8                    + tile[ty + 1][tx + 2] +
                      tile[ty + 2][tx] + tile[ty + 2][tx + 1] + tile[ty + 2][tx + 2];

    int state;

    switch (alive) {
    case 2:  state = tile[ty + 1][tx + 1]; break;
    case 3:  state = GOL_STATE_ALIVE;      break;
    default: state = GOL_STATE_DEAD;
    }

    if (y < height && x < width) { next[(y * width) + x] = state; }
}
}

namespace cudagol {
grid::grid() {
}

grid::~grid() {
}

void grid::create(int width, int height) {
    m_width  = width;
    m_height = height;
    m_bytes  = width * height * sizeof(gol::cell);

    cudaMalloc(&m_d_current, m_bytes);
    cudaMalloc(&m_d_next,    m_bytes);

    m_grid  = dim3(ceil(width / (float)(BLOCK_WIDTH - 2)), ceil(height / (float)(BLOCK_HEIGHT - 2)));
    m_block = dim3(BLOCK_WIDTH, BLOCK_HEIGHT);
}

void grid::destroy() {
    cudaFree(m_d_next);
    cudaFree(m_d_current);
}

void grid::read(gol::cell *data) const {
    cudaMemcpy(data, m_d_current, m_bytes, cudaMemcpyDeviceToHost);
}

void grid::write(gol::cell const *data) const {
    cudaMemcpy(m_d_current, data, m_bytes, cudaMemcpyHostToDevice);
}

void grid::swapBuffers() {
    gol::cell *tmp = m_d_current;
    m_d_current = m_d_next;
    m_d_next = tmp;
}

void grid::singleStep() {
    kernelGOLNaive<BLOCK_WIDTH, BLOCK_HEIGHT><<<m_grid, m_block>>>(m_d_current, m_d_next, m_width, m_height);
    swapBuffers();
}

int grid::width() const {
    return m_width;
}

int grid::height() const {
    return m_height;
}

int grid::bytes() const {
    return m_bytes;
}

gol::cell *grid::d_current() const {
    return m_d_current;
}

gol::cell *grid::d_next() const {
    return m_d_next;
}
}
