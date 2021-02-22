
#include "openglrender2d.h"
#include "golhelpers.h"
#include "utilities.h"

u8 const OpenGLRender2D::s_fill_color = 0xC0;

char const *OpenGLRender2D::s_vertex_shader =
    "#version 330 core\n"
    "layout (location = 0) in vec2 position;\n"
    "layout (location = 1) in vec3 texture_uv;\n"
    "out vec3 uv;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(position.x, position.y, 0.0, 1.0);\n"
    "    uv = texture_uv;\n"
    "}\n";

char const *OpenGLRender2D::s_fragment_shader =
    "#version 330 core\n"
    "out vec3 color;\n"
    "in vec3 uv;\n"
    "uniform sampler3D texture_unit;\n"
    "void main() {\n"
    "    color = texture(texture_unit, uv).rrr;\n"
    "}\n";

OpenGLRender2D::OpenGLRender2D(QWidget *parent) : QOpenGLWidget(parent) {
}

OpenGLRender2D::~OpenGLRender2D() {
    makeCurrent();
    deleteShaderProgram();
    doneCurrent();
}

void OpenGLRender2D::createShaderProgram() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &s_vertex_shader, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &s_fragment_shader, NULL);
    glCompileShader(fs);

    m_sp = glCreateProgram();

    glAttachShader(m_sp, vs);
    glAttachShader(m_sp, fs);

    glLinkProgram(m_sp);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(m_sp);

    GLint location = glGetUniformLocation(m_sp, "texture_unit");
    glUniform1i(location, 0);

    glClearColor(s_fill_color / 255.f, s_fill_color / 255.f, s_fill_color / 255.f, 1.0f);
}

void OpenGLRender2D::deleteShaderProgram() {
    glDeleteProgram(m_sp);
}

void OpenGLRender2D::setOrthographic(float l, float r, float b, float t, float n, float f) {
    GLfloat projection[4][4] = {
        {     2.0f / (r - l),               0.0f,               0.0f, 0.0f },
        {               0.0f,     2.0f / (t - b),               0.0f, 0.0f },
        {               0.0f,               0.0f,    -2.0f / (f - n), 0.0f },
        { -(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0f }
    };

    GLint location = glGetUniformLocation(m_sp, "projection");

    glUniformMatrix4fv(location, 1, GL_FALSE, &projection[0][0]);
}

void OpenGLRender2D::pushQuad(std::vector<float> &v, std::vector<unsigned int> &i, int row, int col) const {
    // vertex format is (x, y, u, v, w)
    // (x, y): 3D space coordinates, z is always 0
    // (u, v, w): 3D texture coordinates

    int tile = (row * m_cols) + col;

    float x1 = col * m_TILE_L; float x2 = x1 + m_TILE_L;
    float y1 = row * m_TILE_L; float y2 = y1 + m_TILE_L;

    float w = (tile + 0.5f) / m_layers;

    v.push_back(x2); v.push_back(y2); v.push_back(1.0f); v.push_back(1.0f); v.push_back(w);
    v.push_back(x2); v.push_back(y1); v.push_back(1.0f); v.push_back(0.0f); v.push_back(w);
    v.push_back(x1); v.push_back(y1); v.push_back(0.0f); v.push_back(0.0f); v.push_back(w);
    v.push_back(x1); v.push_back(y2); v.push_back(0.0f); v.push_back(1.0f); v.push_back(w);

    unsigned int base0 = tile  * 4;
    unsigned int base1 = base0 + 1;
    unsigned int base2 = base0 + 2;
    unsigned int base3 = base0 + 3;

    i.push_back(base0); i.push_back(base1); i.push_back(base3);
    i.push_back(base1); i.push_back(base2); i.push_back(base3);
}

void OpenGLRender2D::createVertexBuffer() {
    glGenVertexArrays(1, &m_vao);

    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(m_tiles * 5 * 4);
    indices.reserve(m_triangles);

    for (int row = 0; row < m_rows; ++row) {
    for (int col = 0; col < m_cols; ++col) { pushQuad(vertices, indices, row, col); }
    }

    glBindBuffer(GL_ARRAY_BUFFER,         m_vbo); glBufferData(GL_ARRAY_BUFFER,         vertices.size() * sizeof(float),        vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()  * sizeof(unsigned int), indices.data(),  GL_STATIC_DRAW);

    int stride = 5 * sizeof(float);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);                   glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(2 * sizeof(float))); glEnableVertexAttribArray(1);
}

void OpenGLRender2D::deleteVertexBuffer() {
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void OpenGLRender2D::createTexture(GLuint *tex) {
    glGenTextures(1, tex);

    glBindTexture(GL_TEXTURE_3D, *tex);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage3D(GL_TEXTURE_3D, 1, GL_R8, m_TILE_L, m_TILE_L, m_layers);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_TILE_L, m_TILE_L, m_layers, GL_RED, GL_UNSIGNED_BYTE, m_bufsw);
}

void OpenGLRender2D::copyTextureToGrid(GLuint tex, char *data) {
    glBindTexture(GL_TEXTURE_3D, tex);
    glGetTexImage(GL_TEXTURE_3D, 0, GL_RED, GL_UNSIGNED_BYTE, m_bufsw);
    utilities::reshapeArray2DTexture3D(m_width, m_height, m_TILE_L, m_bufsw, data, true);
}

void OpenGLRender2D::copyGridToTexture(GLuint tex, char *data) {
    utilities::reshapeArray2DTexture3D(m_width, m_height, m_TILE_L, m_bufsw, data, false);
    glBindTexture(GL_TEXTURE_3D, tex);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, m_TILE_L, m_TILE_L, m_layers, GL_RED, GL_UNSIGNED_BYTE, m_bufsw);
}

void OpenGLRender2D::createGrid(int width, int height, int TILE_L) {
    m_width  = width;
    m_height = height;
    m_TILE_L = TILE_L;

    utilities::tile(width, height, TILE_L, m_rows, m_cols, m_tiles);

    m_triangles = m_tiles * 3 * 2;
    m_layers    = utilities::roundPOT(m_tiles);

    createVertexBuffer();

    int texture_bytes = m_TILE_L * m_TILE_L * m_layers;

    m_bufsw = new char[texture_bytes];
    memset(m_bufsw, s_fill_color, texture_bytes);
}

void OpenGLRender2D::destroyGrid() {
    delete [] m_bufsw;
    deleteVertexBuffer();
}

void OpenGLRender2D::initialize() {
    initializeOpenGLFunctions();
    createShaderProgram();
}

void OpenGLRender2D::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, m_triangles, GL_UNSIGNED_INT, (void *)0);
}

void OpenGLRender2D::beginPaint() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void OpenGLRender2D::endPaint() {
    glUseProgram(m_sp);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

int OpenGLRender2D::TILE_L() const {
    return m_TILE_L;
}

int OpenGLRender2D::width() const {
    return m_width;
}

int OpenGLRender2D::height() const {
    return m_height;
}

//*****************************************************************************
// Begin Renderers
//*****************************************************************************

//-----------------------------------------------------------------------------
// GOLGrid
//-----------------------------------------------------------------------------

void OpenGLRender2D::createGOLGrid() {
    m_cpugolgrid.create(m_width, m_height);
    m_cpugolgrid.setFetch(&gol::fetchWrap);
}

void OpenGLRender2D::writeGOLGrid(gol::cell const *data) {
    m_cpugolgrid.write(data);
}

void OpenGLRender2D::readGOLGrid(gol::cell *data) {
    m_cpugolgrid.read(data);
}

void OpenGLRender2D::singleStepGOLGrid() {
    m_cpugolgrid.singleStep();
}

void OpenGLRender2D::destroyGOLGrid() {
    m_cpugolgrid.destroy();
}

//-----------------------------------------------------------------------------
// GOLGridRenderGL
//-----------------------------------------------------------------------------

void OpenGLRender2D::createGOLGridRenderGL() {
    createTexture(&m_texsw);
}

void OpenGLRender2D::readGOLGridRenderGL(gol::cell *data) {
    copyTextureToGrid(m_texsw, data);
}

void OpenGLRender2D::renderGOLGridRenderGL() {
    copyGridToTexture(m_texsw, m_cpugolgrid.current());
}

void OpenGLRender2D::destroyGOLGridRenderGL() {
    glDeleteTextures(1, &m_texsw);
}

//-----------------------------------------------------------------------------
// CudaGOLGrid
//-----------------------------------------------------------------------------

void OpenGLRender2D::createCudaGOLGrid() {
    m_cudagolgrid.create(m_width, m_height);
}

void OpenGLRender2D::writeCudaGOLGrid(gol::cell const *data) {
    m_cudagolgrid.write(data);
}

void OpenGLRender2D::readCudaGOLGrid(gol::cell *data) {
    m_cudagolgrid.read(data);
}

void OpenGLRender2D::singleStepCudaGOLGrid() {
    m_cudagolgrid.singleStep();
}

void OpenGLRender2D::destroyCudaGOLGrid() {
    m_cudagolgrid.destroy();
}

//-----------------------------------------------------------------------------
// CudaGOLRenderGL
//-----------------------------------------------------------------------------

void OpenGLRender2D::createCudaGOLRenderGL() {
    m_cudagolrendergl.setGrid(m_width, m_height, m_TILE_L / 4, 1024 / (m_TILE_L / 4));
    createTexture(&m_texgl);
    m_cudagolrendergl.registerTexture(m_TILE_L, m_cols, m_texgl);
}

void OpenGLRender2D::readCudaGOLRenderGL(gol::cell *data) {
    copyTextureToGrid(m_texgl, data);
}

void OpenGLRender2D::renderCudaGOLRenderGL() {
    m_cudagolrendergl.render(m_cudagolgrid.d_current());
    glBindTexture(GL_TEXTURE_3D, m_texgl);
}

void OpenGLRender2D::destroyCudaGOLRenderGL() {
    m_cudagolrendergl.unregisterTexture();
    glDeleteTextures(1, &m_texgl);
}

//-----------------------------------------------------------------------------
// CudaGOLGridGL
//-----------------------------------------------------------------------------

void OpenGLRender2D::createCudaGOLGridGL() {
    m_cudagolgridgl.setDimensions(m_width, m_height);
    createTexture(&m_texdb[1]);
    createTexture(&m_texdb[0]);
    m_cudagolgridgl.registerTextures(m_TILE_L, m_cols, m_texdb[0], m_texdb[1]);
}

void OpenGLRender2D::writeCudaGOLGridGL(gol::cell *data) {
    copyGridToTexture(m_texdb[0], data);
}

void OpenGLRender2D::readCudaGOLGridGL(gol::cell *data) {
    copyTextureToGrid(m_texdb[0], data);
}

void OpenGLRender2D::singleStepRenderCudaGOLGridGL() {
    m_cudagolgridgl.singleStepRender();

    GLuint tmp = m_texdb[0];
    m_texdb[0] = m_texdb[1];
    m_texdb[1] = tmp;

    glBindTexture(GL_TEXTURE_3D, m_texdb[0]);
}

void OpenGLRender2D::destroyCudaGOLGridGL() {
    m_cudagolgridgl.unregisterTextures();
    glDeleteTextures(2, m_texdb);
}

//*****************************************************************************
// End Renderers
//*****************************************************************************
