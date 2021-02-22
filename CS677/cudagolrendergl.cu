
#include "cudagolrendergl.h"
#include "utilities.h"

namespace {
template<typename T>
__global__ void kernelGOLRenderGL(gol::cell const *current, cudaSurfaceObject_t surface, int cols, int width, int height, int shr, int mask) {
    int x = ((blockIdx.x * blockDim.x) + threadIdx.x) * sizeof(T);
    int y =  (blockIdx.y * blockDim.y) + threadIdx.y;

    if (y < height && x < width) { surf3Dwrite(*(T *)(current + (y * width) + x), surface, x & mask, y & mask, ((y >> shr) * cols) + (x >> shr)); }
}
}

namespace cudagol {
RenderGL::RenderGL() {
}

RenderGL::~RenderGL() {
}

void RenderGL::setGrid(int width, int height, int BLOCK_WIDTH, int BLOCK_HEIGHT) {
    m_width  = width;
    m_height = height;

    switch (width & 3) {
    case 0:  m_mode = 2; break;
    case 1:  m_mode = 0; break;
    case 2:  m_mode = 1; break;
    default: m_mode = 0;
    }

    m_grid  = dim3(ceil(width  / (double)(BLOCK_WIDTH << m_mode)), ceil(height / (double)BLOCK_HEIGHT));
    m_block = dim3(BLOCK_WIDTH, BLOCK_HEIGHT);
}

void RenderGL::registerTexture(int TILE_L, int cols, GLuint tex) {
    m_cols = cols;
    utilities::divPOTshrmask(TILE_L, m_shr, m_mask);

    cudaGraphicsGLRegisterImage(&m_tex, tex, GL_TEXTURE_3D, cudaGraphicsMapFlagsWriteDiscard);
}

void RenderGL::unregisterTexture() {
    cudaGraphicsUnregisterResource(m_tex);
}

void RenderGL::createSurface() {
    cudaGraphicsMapResources(1, &m_tex);
    cudaArray_t cudaArr;
    cudaGraphicsSubResourceGetMappedArray(&cudaArr, m_tex, 0, 0);
    cudaResourceDesc cudaRD;
    cudaRD.resType = cudaResourceTypeArray;
    cudaRD.res.array.array = cudaArr;
    cudaCreateSurfaceObject(&m_surf, &cudaRD);
}

void RenderGL::deleteSurface() {
    cudaDestroySurfaceObject(m_surf);
    cudaGraphicsUnmapResources(1, &m_tex);
}

void RenderGL::render(gol::cell *d_current) {
    createSurface();
    switch (m_mode) {
    case 0: kernelGOLRenderGL<u8 ><<<m_grid, m_block>>>(d_current, m_surf, m_cols, m_width, m_height, m_shr, m_mask); break;
    case 1: kernelGOLRenderGL<u16><<<m_grid, m_block>>>(d_current, m_surf, m_cols, m_width, m_height, m_shr, m_mask); break;
    case 2: kernelGOLRenderGL<u32><<<m_grid, m_block>>>(d_current, m_surf, m_cols, m_width, m_height, m_shr, m_mask); break;
    }
    deleteSurface();
}

int RenderGL::width() const {
    return m_width;
}

int RenderGL::height() const {
    return m_height;
}

int RenderGL::cols() const {
    return m_cols;
}

int RenderGL::shr() const {
    return m_shr;
}

int RenderGL::mask() const {
    return m_mask;
}

int RenderGL::mode() const {
    return m_mode;
}
}
