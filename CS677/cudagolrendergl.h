
#ifndef CUDAGOLRENDERGL_H
#define CUDAGOLRENDERGL_H

#include <cuda_gl_interop.h>
#include "goltypes.h"

namespace cudagol {
class RenderGL {
private:
    int m_width;
    int m_height;
    int m_cols;
    int m_shr;
    int m_mask;
    int m_mode;

    dim3 m_grid;
    dim3 m_block;

    cudaGraphicsResource_t m_tex;
    cudaSurfaceObject_t    m_surf;

    void createSurface();
    void deleteSurface();

public:
    RenderGL();
    ~RenderGL();

    void setGrid(int width, int height, int BLOCK_WIDTH, int BLOCK_HEIGHT);
    void registerTexture(int TILE_L, int cols, GLuint tex);
    void unregisterTexture();

    void render(gol::cell *d_current);

    int width() const;
    int height() const;
    int cols() const;
    int shr() const;
    int mask() const;
    int mode() const;
};
}

#endif // CUDAGOLRENDERGL_H
