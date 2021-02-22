
#include "cudagolgridgl.h"
#include "utilities.h"

#define MASK_WIDTH  32
#define MASK_HEIGHT 16

namespace {
__global__ void kernelGOLNaive(cudaSurfaceObject_t current, cudaSurfaceObject_t next, int width, int height, int cols, int shr, int mask) {
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

    surf3Dread(&tile[ty][tx], current, lx & mask, ly & mask, ((ly >> shr) * cols) + (lx >> shr));

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

    if (y < height && x < width) { surf3Dwrite((gol::cell)state, next, x & mask, y & mask, ((y >> shr) * cols) + (x >> shr)); }
}
}

namespace cudagol {
gridGL::gridGL() {
}

gridGL::~gridGL() {
}

void gridGL::setDimensions(int width, int height) {
    m_width = width;
    m_height = height;

    m_grid  = dim3(ceil(width / (float)(MASK_WIDTH - 2)), ceil(height / (float)(MASK_HEIGHT - 2)));
    m_block = dim3(MASK_WIDTH, MASK_HEIGHT);
}

void gridGL::registerTextures(int TILE_L, int cols, GLuint tex1, GLuint tex2) {
    m_cols = cols;
    utilities::divPOTshrmask(TILE_L, m_shr, m_mask);

    cudaGraphicsGLRegisterImage(&m_tex[0], tex1, GL_TEXTURE_3D, cudaGraphicsMapFlagsNone);
    cudaGraphicsGLRegisterImage(&m_tex[1], tex2, GL_TEXTURE_3D, cudaGraphicsMapFlagsNone);
}

void gridGL::unregisterTextures() {
    cudaGraphicsUnregisterResource(m_tex[1]);
    cudaGraphicsUnregisterResource(m_tex[0]);
}

void gridGL::createSurface(cudaGraphicsResource_t tex, cudaSurfaceObject_t *surf) {
    cudaArray_t cudaArr;
    cudaResourceDesc cudaRD;

    cudaGraphicsSubResourceGetMappedArray(&cudaArr, tex, 0, 0);
    cudaRD.resType = cudaResourceTypeArray;
    cudaRD.res.array.array = cudaArr;
    cudaCreateSurfaceObject(surf, &cudaRD);
}

void gridGL::createSurfaces() {
    cudaGraphicsMapResources(2, m_tex);
    createSurface(m_tex[0], &m_surf[0]);
    createSurface(m_tex[1], &m_surf[1]);
}

void gridGL::deleteSurfaces() {
    cudaDestroySurfaceObject(m_surf[1]);
    cudaDestroySurfaceObject(m_surf[0]);
    cudaGraphicsUnmapResources(2, m_tex);
}

void gridGL::setFlags() {
    cudaGraphicsResourceSetMapFlags(m_tex[0], cudaGraphicsMapFlagsReadOnly);
    cudaGraphicsResourceSetMapFlags(m_tex[1], cudaGraphicsMapFlagsWriteDiscard);
}

void gridGL::swapBuffers() {
    cudaGraphicsResource_t tmp = m_tex[0];
    m_tex[0] = m_tex[1];
    m_tex[1] = tmp;
}

void gridGL::singleStepRender() {
    setFlags();
    createSurfaces();
    kernelGOLNaive<<<m_grid, m_block>>>(m_surf[0], m_surf[1], m_width, m_height, m_cols, m_shr, m_mask);
    deleteSurfaces();
    swapBuffers();
}
}
