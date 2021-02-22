
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include <QPen>
#include "openglrender2d.h"
#include "goltypes.h"
#include "goltiming.h"
#include "cudagoltiming.h"

enum GOL_RENDERER {
    GOL_RENDERER_SOFTWARE,
    GOL_RENDERER_GL,
    GOL_RENDERER_COMBINED
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    static int    const s_grid_L;
    static int    const s_pen_width;
    static int    const s_TILE_L;
    static double const s_FPS_period;
    static int    const s_profile_runs;
    static double const s_profile_timeout;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    OpenGLRender2D *m_oglr2d;
    QScrollBar     *m_hsb;
    QScrollBar     *m_vsb;
    QPen            m_pen;

    GOL_RENDERER m_renderer;

    gol::cell *m_grid;

    cudagol::timing m_cudatiming;

    bool   m_animate;
    int    m_timecount;
    double m_timeaccum;
    double m_fps;

    int m_viewscale;
    int m_viewx;
    int m_viewy;

    void onInitializeGL();
    void onPaintGL();
    void onResizeGL(int width, int height);

    void fpsCounter();
    void animate();
    void drawGridLines();

    void createCanvas(int width, int height);
    void destroyCanvas();

    void createBuffer();
    void destroyBuffer();

    void createRenderer();
    char const *getRendererName();
    void readGrid();
    void readGridTexture();
    void writeGrid();
    void singleStepGrid();
    void renderGrid();
    void destroyRenderer();

    void showError(QString text);
    void updateTitle();

    void setRenderer(GOL_RENDERER renderer);

    void singleStepRenderGOLGrid();
    void singleStepRenderCudaGOLGrid();
    void singleStepRenderCudaGOLGridGL();
    void singleStepRenderGrid();

    void timeRendererMethod(void (OpenGLRender2D::*f)(), int &n, double &ms);
    QString benchmarkResultString(int n, double ms);
    void benchmarkGOLGrid();
    void benchmarkCudaGOLGrid();
    void benchmarkCudaGOLGridGL();

    void setView(int x, int y, int width, int height, float scale);
    void setView();
    void connectScrollBar();
    void disconnectScrollBar();
    void onHorizontalScroll(int value);
    void onVerticalScroll(int value);
    void resetView();
    void setMaximumScrollBar(QScrollBar *sb, int maximum);
    void setViewRange(int width, int height);
    void resetViewFit();
    void resetViewFancy();

    void loadRandom(int width, int height);

private slots:
    void on_action_file_save_triggered();
    void on_action_file_load_triggered();
    void on_action_file_exit_triggered();

    void on_action_renderer_software_triggered();
    void on_action_renderer_GL_triggered();
    void on_action_renderer_combined_triggered();

    void on_action_simulation_single_step_triggered();
    void on_action_simulation_validate_triggered();
    void on_action_simulation_continuous_triggered();
    void on_action_simulation_benchmark_triggered();

    void on_action_view_fit_triggered();
    void on_action_view_fancy_triggered();

    void on_action_presets_glider_triggered();
    void on_action_presets_1k_triggered();
    void on_action_presets_2k_triggered();
    void on_action_presets_4k_triggered();
    void on_action_presets_8k_triggered();
    void on_action_presets_16k_triggered();
    void on_action_presets_32k_triggered();
    void on_action_presets_canvas_triggered();
};

#endif // MAINWINDOW_H
