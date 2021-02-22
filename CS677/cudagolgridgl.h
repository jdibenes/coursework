
#ifndef CUDAGOLGRIDGL_H
#define CUDAGOLGRIDGL_H

#include <cuda_gl_interop.h>
#include "goltypes.h"

namespace cudagol {
class gridGL {
private:
    int m_width;
    int m_height;
    int m_cols;
    int m_shr;
    int m_mask;

    dim3 m_grid;
    dim3 m_block;

    cudaGraphicsResource_t m_tex[2];
    cudaSurfaceObject_t m_surf[2];

    void createSurface(cudaGraphicsResource_t tex, cudaSurfaceObject_t *surf);
    void createSurfaces();
    void deleteSurfaces();
    void setFlags();
    void swapBuffers();

public:
    gridGL();
    ~gridGL();

    void setDimensions(int width, int height);
    void registerTextures(int TILE_L, int cols, GLuint tex1, GLuint tex2);
    void unregisterTextures();

    void singleStepRender();

    int width() const;
    int height() const;
    int cols() const;
    int shr() const;
    int mask() const;
};
}

#endif // CUDAGOLGRIDGL_H
