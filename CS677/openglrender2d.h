
#ifndef OPENGLRENDER2D_H
#define OPENGLRENDER2D_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include "golgrid.h"
#include "cudagolgrid.h"
#include "cudagolrendergl.h"
#include "cudagolgridgl.h"

class OpenGLRender2D : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT

public:
    static u8 const s_fill_color;

    OpenGLRender2D(QWidget *parent);
    ~OpenGLRender2D();

    void setOrthographic(float l, float r, float b, float t, float n, float f);

    void createGrid(int width, int height, int TILE_L);
    void destroyGrid();

    void initialize();
    void render();
    void beginPaint();
    void endPaint();

    void createGOLGrid();
    void writeGOLGrid(gol::cell const *data);
    void readGOLGrid(gol::cell *data);
    void singleStepGOLGrid();
    void destroyGOLGrid();

    void createGOLGridRenderGL();
    void readGOLGridRenderGL(gol::cell *data);
    void renderGOLGridRenderGL();
    void destroyGOLGridRenderGL();

    void createCudaGOLGrid();
    void writeCudaGOLGrid(gol::cell const *data);
    void readCudaGOLGrid(gol::cell *data);
    void singleStepCudaGOLGrid();
    void destroyCudaGOLGrid();

    void createCudaGOLRenderGL();
    void readCudaGOLRenderGL(gol::cell *data);
    void renderCudaGOLRenderGL();
    void destroyCudaGOLRenderGL();

    void createCudaGOLGridGL();
    void writeCudaGOLGridGL(gol::cell *data);
    void readCudaGOLGridGL(gol::cell *data);
    void singleStepRenderCudaGOLGridGL();
    void destroyCudaGOLGridGL();

    int TILE_L() const;
    int width() const;
    int height() const;

Q_SIGNALS:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private:
    static char const *s_vertex_shader;
    static char const *s_fragment_shader;

    int m_TILE_L;
    int m_width;
    int m_height;
    int m_rows;
    int m_cols;
    int m_tiles;
    int m_triangles;
    int m_layers;

    char *m_bufsw;

    GLuint m_sp;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLuint m_texsw;
    GLuint m_texgl;
    GLuint m_texdb[2];

    gol::grid         m_cpugolgrid;
    cudagol::grid     m_cudagolgrid;
    cudagol::RenderGL m_cudagolrendergl;
    cudagol::gridGL   m_cudagolgridgl;

    void createShaderProgram();
    void deleteShaderProgram();
    void pushQuad(std::vector<float> &v, std::vector<unsigned int> &i, int row, int col) const;
    void createVertexBuffer();
    void deleteVertexBuffer();
    void createTexture(GLuint *tex);
    void copyTextureToGrid(GLuint tex, char *data);
    void copyGridToTexture(GLuint tex, char *data);
};

#endif // OPENGLRENDER2D_H
