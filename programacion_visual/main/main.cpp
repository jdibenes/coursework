//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// main.cpp
// Punto de entrada y ciclo principal del programa
//-------------------------------------------------------------------------------------------------

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <CommCtrl.h>
#include <opencv2\opencv.hpp>
#include "resource.h"
#include "sudokuvisual.h"
#include "profiler.h"

#define X38_NOMBREMATERIA    "Programacion Visual"
#define X38_NOMBRETIPO       "Proyecto OpenCV"
#define X38_NOMBREAPLICACION "Solucionador de sudoku X38"
#define X38_VERSIONSTR       "v1.3"
#define X38_FECHACOMPILACION __DATE__
#define X38_AUTOR            "Juan Carlos Dibene Simental"
#define X38_NUMEROCONTROL    "11210637"

#define X38_VENTANATXTBASE   X38_NOMBREAPLICACION " " X38_VERSIONSTR
#define X38_VENTANACAMARA    "[Camara]"   " - " X38_VENTANATXTBASE
#define X38_VENTANACONSOLA   "[Consola]"  " - " X38_VENTANATXTBASE
#define X38_VENTANASOLUCION  "[Solucion]" " - " X38_VENTANATXTBASE
#define X38_VENTANADEMO      "[Demo]"     " - " X38_VENTANATXTBASE

#define X38_MOSTRARIMAGEN    WM_USER + 0
#define X38_MOSTRARESTADO    WM_USER + 1
#define X38_MOSTRARSOLUCION  WM_USER + 2

bool   g_ejecutar;
HANDLE g_hiloloop;
DWORD  g_hiloloopid;
double g_frecdes;
bool   g_estable;
bool   g_mostrardemo;
HANDLE g_esperardemo;
bool   g_habilitarespera;
bool   g_demolisto;

cv::VideoCapture *g_camara;
cv::Mat           g_framebuffer;

WNDPROC g_camdefwndproc;
WNDPROC g_soldefwndproc;
WNDPROC g_demdefwndproc;
HWND    g_ventanacamara;
HWND    g_statusbar;
HWND    g_ventanasolucion;
HWND    g_ventanademo;

void EnviarImagenDemo(cv::Mat const &imagen)
{
    cv::Mat *buf = new(std::nothrow) cv::Mat(imagen);
    if (buf != NULL) {SendNotifyMessage(g_ventanademo, X38_MOSTRARIMAGEN, 0, (LPARAM)buf);}
}

void Delay(unsigned long ms)
{
    Sleep(ms);
}

void IniciarEspera()
{
    if (g_habilitarespera) {ResetEvent(g_esperardemo);}
}

void Esperar()
{
    if (g_habilitarespera) {WaitForSingleObject(g_esperardemo, INFINITE);}
}

bool Loop()
{
    cv::Mat  *resultado;
    cv::Mat  *solucion;
    cv::Rect *roirc;
    char     *estadostr;

    double ptot;    
    bool   resuelto;

    resultado = new(std::nothrow) cv::Mat();
    if (resultado == NULL) {return false;}

    roirc = new(std::nothrow) cv::Rect();
    if (roirc == NULL) {return false;}
        
    x38profiler::MarcarInicio();
    if (!g_camara->read(g_framebuffer)) {return false;}
    g_framebuffer.copyTo(*resultado);
    resuelto = x38sudokuvisual::Resolver(g_framebuffer, *resultado, roirc);
    ptot = x38profiler::MarcarFinal();

    SendNotifyMessage(g_ventanacamara, X38_MOSTRARIMAGEN, 0, (LPARAM)resultado);

    bool playdemo = false;

    if (resuelto)
    {
        if (x38sudokuvisual::VerificarEstable())
        {
            if (!g_estable)
            {
                solucion = new (std::nothrow) cv::Mat();
                if (solucion == NULL) {return false;}
                *solucion = cv::Mat(*resultado, *roirc).clone();
                SendNotifyMessage(g_ventanasolucion, X38_MOSTRARSOLUCION, 0, (LPARAM)solucion);
            }
            playdemo = g_mostrardemo && (!g_demolisto || !g_estable);
            g_estable = true; 
            g_demolisto = true;
        }
        else
        {
            g_estable = false;
        }
    }
    else
    {
        g_demolisto = false;
    }

    delete roirc;

    estadostr = new(std::nothrow) char[256];
    if (estadostr == NULL) {return false;}
    sprintf(estadostr, "FPS: %.2f | Estado: %s | Demo: %s", g_frecdes / ptot, resuelto ? (g_estable ? "OK" : "Verificando Solucion") : "Preparado", g_mostrardemo ? "Si" : "No");
    SendNotifyMessage(g_ventanacamara, X38_MOSTRARESTADO, 0, (LPARAM)estadostr);

    if (playdemo) {x38sudokuvisual::Demo(g_framebuffer);}
    return true;
}

LRESULT CALLBACK WndProcSolucion(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        ShowWindow(hWnd, SW_MINIMIZE);
        return 0;
    case X38_MOSTRARSOLUCION:
        ShowWindow(g_ventanasolucion, SW_SHOW);
        cv::imshow(X38_VENTANASOLUCION, *(cv::Mat *)lParam);
        delete (cv::Mat *)lParam;
        return 0;
    }

    return CallWindowProc(g_soldefwndproc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProcCamara(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        ShowWindow(hWnd, SW_MINIMIZE);
        return 0;
    case WM_SIZE:
        SendMessage(g_statusbar, uMsg, wParam, lParam);
        break;
    case X38_MOSTRARIMAGEN:        
        cv::imshow(X38_VENTANACAMARA, *(cv::Mat *)lParam);
        delete (cv::Mat *)lParam;
        return 0;
    case X38_MOSTRARESTADO:
        SendMessage(g_statusbar, SB_SETTEXT, 0, lParam);
        delete [] (char *)lParam;
        return 0;
    }

    return CallWindowProc(g_camdefwndproc, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProcDemo(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        g_mostrardemo = false;
        ShowWindow(hWnd, SW_MINIMIZE);
        return 0;
    case WM_SETFOCUS:
        g_mostrardemo = true;
        break;
    case WM_KEYUP:
        if (wParam == VK_SPACE) {SetEvent(g_esperardemo);}
        break;
    case X38_MOSTRARIMAGEN:
        cv::imshow(X38_VENTANADEMO, *(cv::Mat *)lParam);
        delete (cv::Mat *)lParam;
        return 0;
    }

    return CallWindowProc(g_demdefwndproc, hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI LoopEntry(void *)
{
    while (g_ejecutar && Loop());
    return 0;
}

bool Init()
{
    HINSTANCE hinstance;
    HICON     hicon;
    HWND      hwndcon;

    long estilo;
    RECT wndrc;
    RECT stbrc;
    long stbdy;
    UINT banderas;

    hwndcon = GetConsoleWindow();

    SetConsoleCtrlHandler(NULL, TRUE);

    GetSystemMenu(hwndcon, TRUE);
    EnableMenuItem(GetSystemMenu(hwndcon, FALSE), SC_CLOSE, MF_GRAYED);
    
    if (!x38sudokuvisual::Init()) {return false;}
    x38profiler::Init();
    
    g_camara = new(std::nothrow) cv::VideoCapture(0);
    if (g_camara == 0 || !g_camara->isOpened()) {return false;}

    if (!g_camara->read(g_framebuffer)) {return false;}
    
    g_hiloloop = CreateThread(0, 0, LoopEntry, 0, CREATE_SUSPENDED, &g_hiloloopid);
    if (g_hiloloop == NULL) {return false;}    
    SetThreadPriority(g_hiloloop, THREAD_PRIORITY_HIGHEST);

    g_esperardemo = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_esperardemo == NULL) {return false;}
    
    InitCommonControls();

    hicon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(X38_ICONO));

    SetWindowText(hwndcon, X38_VENTANACONSOLA);

    SendMessage(hwndcon, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
    SendMessage(hwndcon, WM_SETICON, ICON_BIG,   (LPARAM)hicon);
    
    cv::namedWindow(X38_VENTANACAMARA);
    g_ventanacamara = GetActiveWindow();
    ShowWindow(g_ventanacamara, SW_HIDE);
    
    estilo = GetWindowLongPtr(g_ventanacamara, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

    SetWindowLongPtr(g_ventanacamara, GWL_STYLE, estilo);
    g_camdefwndproc = (WNDPROC)GetWindowLongPtr(g_ventanacamara, GWLP_WNDPROC);
    SetWindowLongPtr(g_ventanacamara, GWLP_WNDPROC, (LONG)&WndProcCamara);

    hinstance   = (HINSTANCE)GetWindowLongPtr(g_ventanacamara, GWLP_HINSTANCE);
    g_statusbar = CreateWindow(STATUSCLASSNAME, 0, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, g_ventanacamara, 0, hinstance, 0);

    SendMessage(g_ventanacamara, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
    SendMessage(g_ventanacamara, WM_SETICON, ICON_BIG,   (LPARAM)hicon);

    cv::imshow(X38_VENTANACAMARA, g_framebuffer);

    GetWindowRect(g_ventanacamara, &wndrc);
    GetWindowRect(g_statusbar, &stbrc);

    stbdy = stbrc.bottom - stbrc.top;
    banderas = SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOCOPYBITS | SWP_SHOWWINDOW;

    SetWindowPos(g_ventanacamara, 0, 0, 0, wndrc.right - wndrc.left, (wndrc.bottom - wndrc.top) + stbdy, banderas);

    cv::namedWindow(X38_VENTANASOLUCION);
    g_ventanasolucion = GetActiveWindow();
    ShowWindow(g_ventanasolucion, SW_HIDE);
    
    estilo = GetWindowLongPtr(g_ventanasolucion, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

    SetWindowLongPtr(g_ventanasolucion, GWL_STYLE, estilo);
    g_soldefwndproc = (WNDPROC)GetWindowLongPtr(g_ventanasolucion, GWLP_WNDPROC);
    SetWindowLongPtr(g_ventanasolucion, GWLP_WNDPROC, (LONG)&WndProcSolucion);

    SendMessage(g_ventanasolucion, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
    SendMessage(g_ventanasolucion, WM_SETICON, ICON_BIG,   (LPARAM)hicon);

    cv::namedWindow(X38_VENTANADEMO);
    g_ventanademo = GetActiveWindow();
    ShowWindow(g_ventanademo, SW_HIDE);

    estilo = GetWindowLongPtr(g_ventanademo, GWL_STYLE) & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

    SetWindowLongPtr(g_ventanademo, GWL_STYLE, estilo);
    g_demdefwndproc = (WNDPROC)GetWindowLongPtr(g_ventanademo, GWLP_WNDPROC);
    SetWindowLongPtr(g_ventanademo, GWLP_WNDPROC, (LONG)&WndProcDemo);

    SendMessage(g_ventanademo, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
    SendMessage(g_ventanademo, WM_SETICON, ICON_BIG,   (LPARAM)hicon);

    g_framebuffer.setTo(cv::Scalar(0, 0, 0, 0));
    cv::imshow(X38_VENTANADEMO, g_framebuffer);

    g_mostrardemo = true;
    ShowWindow(g_ventanademo, SW_SHOW);
    
    g_frecdes = x38profiler::ObtenerFrecuencia();
    g_estable = false;
    g_demolisto = false;
    g_habilitarespera = true;
    
    return true;
}

void Cleanup()
{
    if (g_hiloloop != NULL) {CloseHandle(g_hiloloop);}
    if (g_esperardemo != NULL) {CloseHandle(g_esperardemo);}
    if (g_camara != 0) {delete g_camara;}
    x38sudokuvisual::Close();
    cv::destroyAllWindows();
}

int main()
{
    DWORD codigohilo;

    g_ejecutar = Init();

    if (g_ejecutar)
    {
        std::cout << "-------------------------------------------------------------------------------" << std::endl;
        std::cout << X38_NOMBREMATERIA << " - " << X38_NOMBRETIPO << std::endl;
        std::cout << X38_NOMBREAPLICACION << std::endl;
        std::cout << X38_VERSIONSTR << " [" << X38_FECHACOMPILACION << "]" << std::endl;
        std::cout << X38_AUTOR << " - " << X38_NUMEROCONTROL << std::endl;
        std::cout << "-------------------------------------------------------------------------------" << std::endl;

        std::cout << std::endl;
        std::cout << "Se ha creado el hilo de ejecucion del solucionador (id: 0x" << std::hex << g_hiloloopid << ")" << std::endl;

        ResumeThread(g_hiloloop);
        while ((char)cv::waitKey(0) != VK_ESCAPE);

        g_ejecutar = false;
        g_habilitarespera = false;
        SetEvent(g_esperardemo);        

        std::cout << "Esperando que el hilo de ejecucion del solucionador termine..." << std::endl;
        WaitForSingleObject(g_hiloloop, INFINITE);

        GetExitCodeThread(g_hiloloop, &codigohilo);
        std::cout << "El hilo de ejecucion del solucionador (0x" << std::hex << g_hiloloopid << ") ha terminado con codigo ";
        std::cout << codigohilo << " (0x" << std::hex << codigohilo << ")" << std::endl;
    }
    else
    {
        MessageBox(NULL, "Error al iniciar el programa", "Error", MB_OK | MB_ICONHAND);
    }

    Cleanup();
    return 0;
}

//-------------------------------------------------------------------------------------------------