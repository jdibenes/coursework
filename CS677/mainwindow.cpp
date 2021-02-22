
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openglrender2d.h"
#include "golhelpers.h"

int    const MainWindow::s_grid_L          = 16;
int    const MainWindow::s_pen_width       = 2;
int    const MainWindow::s_TILE_L          = 1024;
double const MainWindow::s_FPS_period      = 1000.0;
int    const MainWindow::s_profile_runs    = 1000;
double const MainWindow::s_profile_timeout = 10000.0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_oglr2d = ui->openGLWidget;
    m_hsb    = ui->horizontalScrollBar;
    m_vsb    = ui->verticalScrollBar;

    connect(ui->openGLWidget, &OpenGLRender2D::initializeGL, this, &MainWindow::onInitializeGL);
    connect(ui->openGLWidget, &OpenGLRender2D::paintGL, this, &MainWindow::onPaintGL);
    connect(ui->openGLWidget, &OpenGLRender2D::resizeGL, this, &MainWindow::onResizeGL);

    m_pen.setColor(QColor::fromRgb(OpenGLRender2D::s_fill_color, OpenGLRender2D::s_fill_color, OpenGLRender2D::s_fill_color));
    m_pen.setWidth(s_pen_width);
}

MainWindow::~MainWindow() {
    m_oglr2d->makeCurrent();
    destroyCanvas();
    m_oglr2d->doneCurrent();
    delete ui;
}

//-----------------------------------------------------------------------------
// initializeGL
//-----------------------------------------------------------------------------

void MainWindow::onInitializeGL() {
    m_oglr2d->initialize();

    ui->action_renderer_software->setChecked(true);
    m_renderer = GOL_RENDERER_SOFTWARE;

    m_animate = false;

    ui->action_view_fit->setChecked(true);
    m_viewscale = 0;

    m_hsb->setEnabled(false);
    m_vsb->setEnabled(false);

    m_hsb->setMinimum(0);
    m_vsb->setMinimum(0);

    setMaximumScrollBar(m_hsb, 0);
    setMaximumScrollBar(m_vsb, 0);

    connectScrollBar();

    QSize size = m_oglr2d->size();

    int width  = size.width();
    int height = size.height();

    if (width  <= 0) { width  = 1; }
    if (height <= 0) { height = 1; }

    createCanvas(width, height);

    gol::loadRandom(m_grid, width, height);

    writeGrid();
    renderGrid();

    resetViewFit();

    updateTitle();
}

//-----------------------------------------------------------------------------
// resizeGL
//-----------------------------------------------------------------------------

void MainWindow::onResizeGL(int width, int height) {
    if (m_viewscale != 0) { setView(m_viewx, m_viewy, width, height, m_viewscale); }
}

//-----------------------------------------------------------------------------
// paintGL
//-----------------------------------------------------------------------------

void MainWindow::fpsCounter() {
    m_cudatiming.stop();
    m_timeaccum += m_cudatiming.ms();
    m_cudatiming.start();
    ++m_timecount;

    if (m_timeaccum < s_FPS_period) { return; }

    m_fps       = (1000.0 * m_timecount) / m_timeaccum;
    m_timeaccum = 0;
    m_timecount = 0;

    updateTitle();
}

void MainWindow::animate() {
    fpsCounter();
    singleStepRenderGrid();
    //m_cudatiming.sync();
    m_oglr2d->update();
}

void MainWindow::drawGridLines() {
    QSize size = m_oglr2d->size();
    int width  = size.width();
    int height = size.height();

    QPainter p;

    m_oglr2d->beginPaint();
    p.begin(m_oglr2d);

    p.setPen(m_pen);

    for (int x = 1; x < width;  x += m_viewscale) { p.drawLine(x, 0, x, height); }
    for (int y = 1; y < height; y += m_viewscale) { p.drawLine(0, y,  width, y); }

    p.end();
    m_oglr2d->endPaint();
}

void MainWindow::onPaintGL() {
    if (m_animate) { animate(); }
    m_oglr2d->render();
    if (m_viewscale > 0) { drawGridLines(); }
}

//-----------------------------------------------------------------------------
// Canvas
//-----------------------------------------------------------------------------

void MainWindow::createCanvas(int width, int height) {
    m_oglr2d->createGrid(width, height, s_TILE_L);
    createRenderer();
    createBuffer();
}

void MainWindow::destroyCanvas() {
    destroyBuffer();
    destroyRenderer();
    m_oglr2d->destroyGrid();
}

//-----------------------------------------------------------------------------
// Renderer
//-----------------------------------------------------------------------------

void MainWindow::createBuffer() {
    m_grid = new gol::cell[m_oglr2d->width() * m_oglr2d->height()];
}

void MainWindow::destroyBuffer() {
    delete [] m_grid;
}

void MainWindow::createRenderer() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: m_oglr2d->createGOLGrid();       m_oglr2d->createGOLGridRenderGL(); break;
    case GOL_RENDERER_GL:       m_oglr2d->createCudaGOLGrid();   m_oglr2d->createCudaGOLRenderGL(); break;
    case GOL_RENDERER_COMBINED: m_oglr2d->createCudaGOLGridGL();                                    break;
    }
}

char const *MainWindow::getRendererName() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: return "CPU Texture Copy";
    case GOL_RENDERER_GL:       return "CUDA Buffer Copy";
    case GOL_RENDERER_COMBINED: return "CUDA Combined";
    }

    return "Unspecified";
}

void MainWindow::readGrid() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: m_oglr2d->readGOLGrid(m_grid);       break;
    case GOL_RENDERER_GL:       m_oglr2d->readCudaGOLGrid(m_grid);   break;
    case GOL_RENDERER_COMBINED: m_oglr2d->readCudaGOLGridGL(m_grid); break;
    }
}

void MainWindow::readGridTexture() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: m_oglr2d->readGOLGridRenderGL(m_grid); break;
    case GOL_RENDERER_GL:       m_oglr2d->readCudaGOLRenderGL(m_grid); break;
    case GOL_RENDERER_COMBINED: m_oglr2d->readCudaGOLGridGL(m_grid);   break;
    }
}

void MainWindow::writeGrid() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: m_oglr2d->writeGOLGrid(m_grid);       break;
    case GOL_RENDERER_GL:       m_oglr2d->writeCudaGOLGrid(m_grid);   break;
    case GOL_RENDERER_COMBINED: m_oglr2d->writeCudaGOLGridGL(m_grid); break;
    }
}

void MainWindow::singleStepGrid() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: m_oglr2d->singleStepGOLGrid();             break;
    case GOL_RENDERER_GL:       m_oglr2d->singleStepCudaGOLGrid();         break;
    case GOL_RENDERER_COMBINED: m_oglr2d->singleStepRenderCudaGOLGridGL(); break;
    }
}

void MainWindow::renderGrid() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: m_oglr2d->renderGOLGridRenderGL(); break;
    case GOL_RENDERER_GL:       m_oglr2d->renderCudaGOLRenderGL(); break;
    case GOL_RENDERER_COMBINED:                                    break;
    }
}

void MainWindow::destroyRenderer() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: m_oglr2d->destroyGOLGridRenderGL(); m_oglr2d->destroyGOLGrid();       break;
    case GOL_RENDERER_GL:       m_oglr2d->destroyCudaGOLRenderGL(); m_oglr2d->destroyCudaGOLGrid();   break;
    case GOL_RENDERER_COMBINED:                                     m_oglr2d->destroyCudaGOLGridGL(); break;
    }
}

//-----------------------------------------------------------------------------
// File
//-----------------------------------------------------------------------------

void MainWindow::updateTitle() {
    QString size = QString::number(m_oglr2d->width()) + "x" + QString::number(m_oglr2d->height());
    QString fps  = m_animate ? " | " + QString::number(m_fps, 'f', 2) + " FPS" : "";
    QString mode = " | " + QString(getRendererName());
    setWindowTitle(QString("Project - ") + size + fps + mode);
}

void MainWindow::showError(QString text) {
    QMessageBox msgbox;

    msgbox.setWindowTitle("Error");
    msgbox.setText(text);
    msgbox.setIcon(QMessageBox::Critical);

    msgbox.exec();
}

void MainWindow::on_action_file_save_triggered() {
    QFileDialog fd(this, "Save");

    fd.setFileMode(QFileDialog::AnyFile);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setNameFilter("All files (*.*)");
    fd.setDefaultSuffix(".gol");

    if (!fd.exec()) { return; }

    QStringList filenames = fd.selectedFiles();
    QString filename = filenames.at(0);
    std::string string = filename.toUtf8().constData();

    m_oglr2d->makeCurrent();
    readGrid();
    m_oglr2d->doneCurrent();

    if (gol::saveFile(string.c_str(), m_grid, m_oglr2d->width(), m_oglr2d->height())) { return; }
    showError("Failed to write file: " + filename);
}

void MainWindow::on_action_file_load_triggered() {
    QFileDialog fd(this, "Load");

    fd.setFileMode(QFileDialog::ExistingFile);
    fd.setAcceptMode(QFileDialog::AcceptOpen);
    fd.setNameFilter("All files (*.*)");

    if (!fd.exec()) { return; }

    QStringList filenames = fd.selectedFiles();
    QString filename = filenames.at(0);
    std::string string = filename.toUtf8().constData();

    int width;
    int height;

    if (!gol::loadHeader(string.c_str(), width, height)) {
        showError("Failed to read header from: " + filename);
        return;
    }

    m_oglr2d->makeCurrent();

    destroyCanvas();
    createCanvas(width, height);

    if (!gol::loadFile(string.c_str(), m_grid, width, height)) {
        showError("Failed to read grid data from: " + filename);
    }

    gol::sanitize(m_grid, width, height);

    writeGrid();
    renderGrid();
    resetView();

    m_oglr2d->doneCurrent();
    m_oglr2d->update();

    updateTitle();
}

void MainWindow::on_action_file_exit_triggered() {
    qApp->quit();
}

//-----------------------------------------------------------------------------
// Renderer
//-----------------------------------------------------------------------------

void MainWindow::setRenderer(GOL_RENDERER renderer) {
    ui->action_renderer_software->setChecked(renderer == GOL_RENDERER_SOFTWARE);
    ui->action_renderer_GL->setChecked(renderer == GOL_RENDERER_GL);
    ui->action_renderer_combined->setChecked(renderer == GOL_RENDERER_COMBINED);

    if (renderer == m_renderer) { return; }

    m_oglr2d->makeCurrent();

    readGrid();
    destroyRenderer();

    m_renderer = renderer;

    createRenderer();
    writeGrid();
    renderGrid();

    m_oglr2d->doneCurrent();
    m_oglr2d->update();

    updateTitle();
}

void MainWindow::on_action_renderer_software_triggered() {
    setRenderer(GOL_RENDERER_SOFTWARE);
}

void MainWindow::on_action_renderer_GL_triggered() {
    setRenderer(GOL_RENDERER_GL);
}

void MainWindow::on_action_renderer_combined_triggered() {
    setRenderer(GOL_RENDERER_COMBINED);
}

//-----------------------------------------------------------------------------
// Simulation
//-----------------------------------------------------------------------------

void MainWindow::singleStepRenderGOLGrid() {
    m_oglr2d->singleStepGOLGrid();
    m_oglr2d->renderGOLGridRenderGL();
}

void MainWindow::singleStepRenderCudaGOLGrid() {
    m_oglr2d->singleStepCudaGOLGrid();
    m_oglr2d->renderCudaGOLRenderGL();
}

void MainWindow::singleStepRenderCudaGOLGridGL() {
    m_oglr2d->singleStepRenderCudaGOLGridGL();
}

void MainWindow::singleStepRenderGrid() {
    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: singleStepRenderGOLGrid();       break;
    case GOL_RENDERER_GL:       singleStepRenderCudaGOLGrid();   break;
    case GOL_RENDERER_COMBINED: singleStepRenderCudaGOLGridGL(); break;
    }
}

void MainWindow::timeRendererMethod(void (OpenGLRender2D::*f)(), int &n, double &ms) {
    int    times   = s_profile_runs;
    double timeout = s_profile_timeout;

    ms = 0.0;

    for (n = 0; (n < times) && (ms < timeout); ++n) {
        m_cudatiming.start();
        (m_oglr2d->*f)();
        m_cudatiming.stop();
        ms += m_cudatiming.ms();
    }
}

QString MainWindow::benchmarkResultString(int n, double ms) {
    return "Runs: " + QString::number(n) + "\nTotal: " + QString::number(ms) + " ms\nAverage: " + QString::number(ms / n) + " ms";
}

void MainWindow::benchmarkGOLGrid() {
    int    n_ss;
    int    n_rr;
    double t_ss;
    double t_rr;

    timeRendererMethod(&OpenGLRender2D::singleStepGOLGrid, n_ss, t_ss);
    timeRendererMethod(&OpenGLRender2D::renderGOLGridRenderGL, n_rr, t_rr);

    QMessageBox msgBox;

    QString str_tr = "Renderer: " + QString(getRendererName());
    QString str_ss = "Single Step Kernel\n" + benchmarkResultString(n_ss, t_ss);
    QString str_rr = "Render Kernel\n" + benchmarkResultString(n_rr, t_rr);

    msgBox.setWindowTitle("Benchmark Kernel");
    msgBox.setText(str_tr + "\n\n" + str_ss + "\n\n" + str_rr);
    msgBox.setIcon(QMessageBox::Information);

    msgBox.exec();
}

void MainWindow::benchmarkCudaGOLGrid() {
    int    n_ss;
    int    n_rr;
    double t_ss;
    double t_rr;

    timeRendererMethod(&OpenGLRender2D::singleStepCudaGOLGrid, n_ss, t_ss);
    timeRendererMethod(&OpenGLRender2D::renderCudaGOLRenderGL, n_rr, t_rr);

    QMessageBox msgBox;

    QString str_tr = "Renderer: " + QString(getRendererName());
    QString str_ss = "Single Step Kernel\n" + benchmarkResultString(n_ss, t_ss);
    QString str_rr = "Render Kernel\n" + benchmarkResultString(n_rr, t_rr);

    msgBox.setWindowTitle("Benchmark Kernel");
    msgBox.setText(str_tr + "\n\n" + str_ss + "\n\n" + str_rr);
    msgBox.setIcon(QMessageBox::Information);

    msgBox.exec();
}

void MainWindow::benchmarkCudaGOLGridGL() {
    int    n_ss;
    double t_ss;

    timeRendererMethod(&OpenGLRender2D::singleStepRenderCudaGOLGridGL, n_ss, t_ss);

    QMessageBox msgBox;

    QString str_tr = "Renderer: " + QString(getRendererName());
    QString str_ss = "Single Step Render Kernel\n" + benchmarkResultString(n_ss, t_ss);

    msgBox.setWindowTitle("Benchmark Kernel");
    msgBox.setText(str_tr + "\n\n" + str_ss);
    msgBox.setIcon(QMessageBox::Information);

    msgBox.exec();
}

void MainWindow::on_action_simulation_single_step_triggered() {
    m_oglr2d->makeCurrent();
    singleStepRenderGrid();
    m_oglr2d->doneCurrent();
    m_oglr2d->update();
}

void MainWindow::on_action_simulation_validate_triggered() {
    m_oglr2d->makeCurrent();

    gol::grid cpugrid;

    cpugrid.create(m_oglr2d->width(), m_oglr2d->height());
    cpugrid.setFetch(&gol::fetchWrap);

    readGrid();

    cpugrid.write(m_grid);
    cpugrid.singleStep();

    singleStepGrid();
    readGrid();

    bool failgol = memcmp(cpugrid.current(), m_grid, cpugrid.bytes());

    renderGrid();
    readGridTexture();

    bool failtex = memcmp(cpugrid.current(), m_grid, cpugrid.bytes());

    cpugrid.destroy();

    m_oglr2d->doneCurrent();
    m_oglr2d->update();

    QString testgol = failgol ? "Fail" : "Pass";
    QString testtex = failtex ? "Fail" : "Pass";

    QMessageBox msgBox;

    msgBox.setWindowTitle("Single Step Verify");
    msgBox.setText("Renderer: " + QString(getRendererName()) + "\nNext state: " + testgol + "\nRender texture: " + testtex);
    msgBox.setIcon((failgol || failtex) ? QMessageBox::Critical : QMessageBox::Information);

    msgBox.exec();
}

void MainWindow::on_action_simulation_benchmark_triggered() {
    m_oglr2d->makeCurrent();

    switch (m_renderer) {
    case GOL_RENDERER_SOFTWARE: benchmarkGOLGrid();       break;
    case GOL_RENDERER_GL:       benchmarkCudaGOLGrid();   break;
    case GOL_RENDERER_COMBINED: benchmarkCudaGOLGridGL(); break;
    }

    m_oglr2d->doneCurrent();
    m_oglr2d->update();
}

void MainWindow::on_action_simulation_continuous_triggered() {
    ui->action_file_load->setEnabled(m_animate);
    ui->action_file_save->setEnabled(m_animate);
    ui->action_simulation_single_step->setEnabled(m_animate);
    ui->action_simulation_validate->setEnabled(m_animate);
    ui->action_simulation_benchmark->setEnabled(m_animate);

    if (m_animate) { m_cudatiming.stop();  }
    else           { m_cudatiming.start(); }

    m_animate   = !m_animate;
    m_timecount = 0;
    m_timeaccum = 0;
    m_fps       = 0.0;

    m_oglr2d->update();

    updateTitle();
}

//-----------------------------------------------------------------------------
// View
//-----------------------------------------------------------------------------

void MainWindow::setView(int x, int y, int width, int height, float scale) {
    ui->openGLWidget->setOrthographic(x, x + width / scale, y + height / scale, y, 0.0f, 1.0f);
}

void MainWindow::setView() {
    QSize size = ui->openGLWidget->size();
    setView(m_viewx, m_viewy, size.width(), size.height(), m_viewscale);
    ui->statusBar->showMessage("View: (" + QString::number(m_viewx) + "," + QString::number(m_viewy) + ")");
}

void MainWindow::connectScrollBar() {
    connect(m_hsb, &QScrollBar::valueChanged, this, &MainWindow::onHorizontalScroll);
    connect(m_vsb, &QScrollBar::valueChanged, this, &MainWindow::onVerticalScroll);
}

void MainWindow::disconnectScrollBar() {
    disconnect(m_hsb, &QScrollBar::valueChanged, this, &MainWindow::onHorizontalScroll);
    disconnect(m_vsb, &QScrollBar::valueChanged, this, &MainWindow::onVerticalScroll);
}

void MainWindow::onHorizontalScroll(int value) {
    m_viewx = value;

    m_oglr2d->makeCurrent();
    setView();
    m_oglr2d->doneCurrent();
    m_oglr2d->update();
}

void MainWindow::onVerticalScroll(int value) {
    m_viewy = value;

    m_oglr2d->makeCurrent();
    setView();
    m_oglr2d->doneCurrent();
    m_oglr2d->update();
}

void MainWindow::resetView() {
    if      (ui->action_view_fit->isChecked())   { resetViewFit();   }
    else if (ui->action_view_fancy->isChecked()) { resetViewFancy(); }
}

void MainWindow::setMaximumScrollBar(QScrollBar *sb, int maximum) {
    sb->setMaximum(maximum);
    sb->setValue(0);
}

void MainWindow::setViewRange(int width, int height) {
    disconnectScrollBar();
    setMaximumScrollBar(m_hsb, width);
    setMaximumScrollBar(m_vsb, height);
    connectScrollBar();
}

void MainWindow::resetViewFit() {
    m_oglr2d->setOrthographic(0.0f, m_oglr2d->width(), m_oglr2d->height(), 0.0f, 0.0f, 1.0f);
}

void MainWindow::on_action_view_fit_triggered() {
    ui->action_view_fit->setChecked(true);
    ui->action_view_fancy->setChecked(false);

    m_hsb->setEnabled(false);
    m_vsb->setEnabled(false);

    m_viewscale = 0;
    setViewRange(0, 0);

    m_oglr2d->makeCurrent();
    resetViewFit();
    m_oglr2d->doneCurrent();
    m_oglr2d->update();
}

void MainWindow::resetViewFancy() {
    m_viewx = 0;
    m_viewy = 0;

    setViewRange(m_oglr2d->width() - 1, m_oglr2d->height() - 1);
    setView();
}

void MainWindow::on_action_view_fancy_triggered() {
    ui->action_view_fit->setChecked(false);
    ui->action_view_fancy->setChecked(true);

    m_hsb->setEnabled(true);
    m_vsb->setEnabled(true);

    m_viewscale = s_grid_L;

    m_oglr2d->makeCurrent();
    resetViewFancy();
    m_oglr2d->doneCurrent();
    m_oglr2d->update();
}

//-----------------------------------------------------------------------------
// Presets
//-----------------------------------------------------------------------------

void MainWindow::loadRandom(int width, int height) {
    m_oglr2d->makeCurrent();

    destroyCanvas();
    createCanvas(width, height);

    gol::loadRandom(m_grid, width, height);

    writeGrid();
    renderGrid();
    resetView();

    m_oglr2d->doneCurrent();
    m_oglr2d->update();

    updateTitle();
}

void MainWindow::on_action_presets_glider_triggered() {
    int width  = 32;
    int height = 32;

    m_oglr2d->makeCurrent();

    destroyCanvas();
    createCanvas(width, height);

    memset(m_grid, GOL_STATE_DEAD, width * height * sizeof(gol::cell));

    m_grid[1 * width + 3] = GOL_STATE_ALIVE;
    m_grid[2 * width + 3] = GOL_STATE_ALIVE;
    m_grid[3 * width + 3] = GOL_STATE_ALIVE;
    m_grid[3 * width + 2] = GOL_STATE_ALIVE;
    m_grid[2 * width + 1] = GOL_STATE_ALIVE;

    writeGrid();
    renderGrid();
    resetView();

    m_oglr2d->doneCurrent();
    m_oglr2d->update();

    updateTitle();
}

void MainWindow::on_action_presets_1k_triggered() {
    loadRandom(1024, 1024);
}

void MainWindow::on_action_presets_2k_triggered() {
    loadRandom(2048, 2048);
}

void MainWindow::on_action_presets_4k_triggered() {
    loadRandom(4096, 4096);
}

void MainWindow::on_action_presets_8k_triggered() {
    loadRandom(8192, 8192);
}

void MainWindow::on_action_presets_16k_triggered() {
    loadRandom(16384, 16384);
}

void MainWindow::on_action_presets_32k_triggered() {
    loadRandom(32768, 32768);
}

void MainWindow::on_action_presets_canvas_triggered() {
    QSize size = m_oglr2d->size();

    int width  = size.width();
    int height = size.height();

    if (width  < 1) { width  = 1; }
    if (height < 1) { height = 1; }

    loadRandom(width, height);
}
