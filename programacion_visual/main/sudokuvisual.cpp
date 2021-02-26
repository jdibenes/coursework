//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// sudokuvisual.cpp
// Resuelve un sudoku utilizando OpenCV (http://opencv.org/)
//-------------------------------------------------------------------------------------------------

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <x38ocr.h>
#include "sudoku.h"
#include "demo.h"

namespace x38sudokuvisual
{
bool g_init = false;

cv::Mat  g_kernel;
cv::Size g_textsize;

cv::Mat   g_imagen;
cv::Mat   g_roi;
cv::Mat   g_digito;
cv::Rect  g_celda;
cv::Rect  g_maxrc;
cv::Point g_maxpt;

char             g_sudoku[9][9];
std::vector<int> g_libres;

char g_sudokuprev[9][9];
int  g_total;

bool Init()
{
    if ( g_init) {return true;}

    g_init = x38ocr::Init();
    if (!g_init) {return false;}

    g_kernel   = (cv::Mat_<uchar>(3, 3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);

    g_total = 0;
    memset(g_sudokuprev, 0, sizeof(g_sudokuprev));

    return true;
}

void FiltrarImagen()
{
    cv::cvtColor(g_imagen, g_imagen, CV_BGR2GRAY);
    cv::GaussianBlur(g_imagen, g_imagen, cv::Size(5, 5), 0);
    cv::adaptiveThreshold(g_imagen, g_imagen, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 5, 2);
    cv::dilate(g_imagen, g_imagen, g_kernel);
}

void Corregir(char &digito)
{
    if (digito == 'l' || digito == 'I' || digito == '!' || digito == 'i' || digito == 'L') {digito = '1';}
}

bool EncontrarSudoku()
{   
    const double minra   = 0.50;
    const double maxra   = 0.75;
    const double minrhw  = 0.90;
    const double maxrhw  = 1 / minrhw;
    const int    dy      = 10;
    const int    dx      = 10;
    const int    descx   = 50;
    const int    descy   = 50;

    const int    h  = g_imagen.size().height;
    const int    w  = g_imagen.size().width;
    const double a  = h * w;
    const int    cl = (w / 2) - descx;
    const int    ct = (h / 2) - descy;
    const int    cr = (w / 2) + descx;
    const int    cb = (h / 2) + descy;

    double rhw;
    double ra;
    uchar *fil;
    int    scx;
    int    scy;

    for (int y = 0; y < h; y += dy)
    {
        fil = g_imagen.ptr(y);
        g_maxpt.y = y;

        for (int x = 0; x < w; x += dx)
        {
            if (fil[x] == 255)
            {
                g_maxpt.x = x;
                cv::floodFill(g_imagen, g_maxpt, CV_RGB(0, 0, 250), &g_maxrc);
                ra = (double)g_maxrc.area() / a;

                if (ra >= minra && ra <= maxra)
                {
                    rhw = (double)g_maxrc.height / (double)g_maxrc.width;

                    if (rhw >= minrhw && rhw <= maxrhw)
                    {
                        scx = g_maxrc.x + g_maxrc.width  / 2;
                        scy = g_maxrc.y + g_maxrc.height / 2;

                        if (scx >= cl && scx <= cr && scy >= ct && scy <= cb)
                        {
                            cv::floodFill(g_imagen, g_maxpt, CV_RGB(0, 0, 0));
                            return true;
                        }
                    }

                    return false;
                }
            }
        }
    }

    return false;
}

void LlenarSudoku()
{
    const int maxintentos = 5;
    const int escala      = 2;

    char caracter;
    int  intentos;

    cv::Mat(g_imagen, g_maxrc).copyTo(g_roi);
    cv::resize(g_roi, g_roi, g_roi.size() * escala, 0, 0);    

    g_celda.width  = g_roi.size().width  / 9;
    g_celda.height = g_roi.size().height / 9;

    memset(g_sudoku, '0', sizeof(g_sudoku));
    g_libres.clear();

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            g_celda.x = j * g_celda.width;
            g_celda.y = i * g_celda.height;

            g_digito = cv::Mat(g_roi, g_celda).clone();
            intentos = 0;
            
            do
            {
                caracter = x38ocr::LeerCaracter(g_digito.data, g_digito.cols, g_digito.rows);
                Corregir(caracter);
                if (caracter >= '1' && caracter <= '9') {break;}
                if (intentos >= maxintentos)
                {
                    caracter = '0';
                    break;
                }
                cv::erode(g_digito, g_digito, g_kernel);
                intentos++;
            }
            while (true);

            g_sudoku[i][j] = caracter;
            if (caracter == '0') {g_libres.push_back((i * 9) + j);}
        }
    }
}

void LlenarSolucion(cv::Mat &resultado)
{
    const int    celdaw = g_maxrc.width  / 9;
    const int    celdah = g_maxrc.height / 9;
    const double sc     = celdah / 55.0;

    char str[2];
    int  i;
    int  j;
    int  x;
    int  y;
    int  baseline;
    int  ox;
    int  oy;

    str[0] = '0';
    str[1] = '\0';

    g_textsize =  cv::getTextSize(str, cv::FONT_HERSHEY_SIMPLEX, sc, 2, &baseline);

    ox = (celdaw - g_textsize.width ) / 2;
    oy = (celdah - g_textsize.height) / 2;

    for (size_t k = 0; k < g_libres.size(); k++)
    {
        i = g_libres[k] / 9;
        j = g_libres[k] % 9;

        str[0] = g_sudoku[i][j];

        x = g_maxrc.x + ((g_maxrc.width  * j) / 9) + ox;
        y = g_maxrc.y + ((g_maxrc.height * i) / 9) + oy + g_textsize.height;

        cv::putText(resultado, str, cvPoint(x, y), cv::FONT_HERSHEY_SIMPLEX, sc, CV_RGB(0, 0, 0), 2);
    }
}

bool Resolver(cv::Mat const &imagen, cv::Mat &resultado, cv::Rect *roirc)
{
    if (!g_init) {return false;}

    imagen.copyTo(g_imagen);

    FiltrarImagen();
    if (!EncontrarSudoku())             {return false;}
    LlenarSudoku();
    if (!x38sudoku::Resolver(g_sudoku)) {return false;}
    LlenarSolucion(resultado);

    if (roirc != NULL) {*roirc = g_maxrc;}

    return true;
}

bool VerificarEstable()
{
    const int totalestable = 16;

    if (memcmp(g_sudokuprev, g_sudoku, sizeof(g_sudokuprev)) == 0)
    {
        g_total++;
    }
    else
    {
        g_total = 0;
        memcpy(g_sudokuprev, g_sudoku, sizeof(g_sudokuprev));
    }

    return g_total >= totalestable;
}

void Close()
{
    if (g_init) {x38ocr::Close();}
    g_init = false;
}

void Demo(cv::Mat const &imagen)
{
    cv::Mat bg;
    cv::Mat ventana;
    cv::Mat cuadros;
    cv::Rect rc;

    imagen.copyTo(bg);
    imagen.copyTo(ventana);
    imagen.copyTo(cuadros);

    rc = cv::Rect(10, 10, bg.size().width - 20, 70);

    cv::rectangle(ventana, rc, CV_RGB(0, 0, 0), CV_FILLED);
    cv::addWeighted(bg(rc), 0.3, ventana(rc), 0.7, 0.0, cuadros(rc));

    cv::putText(cuadros, "Esta es la imagen obtenida de la camara. A continuacion se filtrara la imagen para buscar",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "algun sudoku contenido en esta.",
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 65), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);

    EnviarImagenDemo(cuadros);

    IniciarEspera();
    Esperar();

    imagen.copyTo(g_imagen);

    FiltrarImagen();

    g_imagen.copyTo(bg);
    g_imagen.copyTo(ventana);
    g_imagen.copyTo(cuadros);

    cv::rectangle(ventana, rc, CV_RGB(0, 0, 0), CV_FILLED);
    cv::addWeighted(bg(rc), 0.3, ventana(rc), 0.7, 0.0, cuadros(rc));

    cv::putText(cuadros, "Esta es la imagen filtrada. Ahora se procedera a buscar el sudoku en la imagen.",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);

    EnviarImagenDemo(cuadros);

    IniciarEspera();
    Esperar();

    bool encontrado = EncontrarSudoku();

    g_imagen.copyTo(bg);
    g_imagen.copyTo(ventana);
    g_imagen.copyTo(cuadros);

    cv::rectangle(ventana, rc, CV_RGB(0, 0, 0), CV_FILLED);

    if (encontrado)
    {
    cv::cvtColor(bg, bg, CV_GRAY2BGR);
    cv::cvtColor(ventana, ventana, CV_GRAY2BGR);
    cv::cvtColor(cuadros, cuadros, CV_GRAY2BGR);
    cv::rectangle(bg, g_maxrc, CV_RGB(255, 255, 0));
    cv::rectangle(cuadros, g_maxrc, CV_RGB(255, 255, 0));
    }

    cv::addWeighted(bg(rc), 0.3, ventana(rc), 0.7, 0.0, cuadros(rc));

    if (!encontrado)
    {
    cv::putText(cuadros, "No se encontro algun sudoku en la imagen.",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    }
    else
    {
    cv::putText(cuadros, "Se ha encontrado el sudoku en la imagen. Ahora hay que extraer los numeros de cada celda.",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    }

    EnviarImagenDemo(cuadros);

    IniciarEspera();
    Esperar();

    if (encontrado)
    {
    LlenarSudoku();

    bg.copyTo(cuadros);
    cv::Rect dc;

    dc.width  = g_maxrc.width  / 9;
    dc.height = g_maxrc.height / 9;

    std::string numeros("");
    numeros.reserve(90);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (g_sudoku[i][j] >= '1' && g_sudoku[i][j] <= '9')
            {
                dc.x = g_maxrc.x + (j * g_maxrc.width ) / 9;
                dc.y = g_maxrc.y + (i * g_maxrc.height) / 9;
                cv::rectangle(bg, dc, CV_RGB(0, 255, 255));
                cv::rectangle(cuadros, dc, CV_RGB(0, 255, 255));
                numeros += g_sudoku[i][j];
                EnviarImagenDemo(cuadros);
                Delay(150);
            }
        }
    }

    cv::addWeighted(bg(rc), 0.3, ventana(rc), 0.7, 0.0, cuadros(rc));

    cv::putText(cuadros, "Los numeros identificados en el sudoku se muestran dentro de un rectangulo. Numeros: ",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, numeros.length() <= 0 ? "(No hay...)" : numeros.c_str(),
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 65), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);

    EnviarImagenDemo(cuadros);

    IniciarEspera();
    Esperar();

    bg.copyTo(cuadros);
    cv::addWeighted(bg(rc), 0.3, ventana(rc), 0.7, 0.0, cuadros(rc));

    cv::putText(cuadros, "Ahora se procedera a resolver el sudoku. Las solucion se escribira en los espacios ",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "vacios del sudoku (Suponiendo que el sudoku sea valido).",
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 65), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);

    EnviarImagenDemo(cuadros);

    IniciarEspera();
    Esperar();

    bool resuelto = x38sudoku::Resolver(g_sudoku);

    bg.copyTo(cuadros);
    numeros = std::string("");
    numeros.reserve(90);

    if (resuelto)
    {
    const int    celdaw = g_maxrc.width  / 9;
    const int    celdah = g_maxrc.height / 9;
    const double sc     = celdah / 55.0;

    char str[2];
    int  i;
    int  j;
    int  x;
    int  y;
    int  baseline;
    int  ox;
    int  oy;

    str[0] = '0';
    str[1] = '\0';

    g_textsize =  cv::getTextSize(str, cv::FONT_HERSHEY_SIMPLEX, sc, 2, &baseline);

    ox = (celdaw - g_textsize.width ) / 2;
    oy = (celdah - g_textsize.height) / 2;

    for (size_t k = 0; k < g_libres.size(); k++)
    {
        i = g_libres[k] / 9;
        j = g_libres[k] % 9;

        str[0] = g_sudoku[i][j];
        numeros += g_sudoku[i][j];

        x = g_maxrc.x + ((g_maxrc.width  * j) / 9) + ox;
        y = g_maxrc.y + ((g_maxrc.height * i) / 9) + oy + g_textsize.height;

        cv::putText(bg, str, cvPoint(x, y), cv::FONT_HERSHEY_SIMPLEX, sc, CV_RGB(255, 255, 255), 2);
        cv::putText(cuadros, str, cvPoint(x, y), cv::FONT_HERSHEY_SIMPLEX, sc, CV_RGB(255, 255, 255), 2);
        EnviarImagenDemo(cuadros);
        Delay(150);
    }
    }

    cv::addWeighted(bg(rc), 0.3, ventana(rc), 0.7, 0.0, cuadros(rc));

    if (!resuelto)
    {
    cv::putText(cuadros, "No se ha encontrado solucion para el sudoku extraido.",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    }
    else
    {
    cv::putText(cuadros, "Se ha encontrado la solucion para el sudoku extraido. Numeros: ",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, numeros.length() <= 0 ? "(No hay...)" : numeros.c_str(),
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 65), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    }

    EnviarImagenDemo(cuadros);

    IniciarEspera();
    Esperar();
    }

    bg.copyTo(cuadros);
    cv::addWeighted(bg(rc), 0.3, ventana(rc), 0.7, 0.0, cuadros(rc));

    cv::putText(cuadros, "Fin de la demostracion.",
                cvPoint(15, 25), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);
    cv::putText(cuadros, "(Presione espacio para continuar...)",
                cvPoint(15, 45), cv::FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255), 1);

    EnviarImagenDemo(cuadros);

    IniciarEspera();
    Esperar();

    cuadros.setTo(cv::Scalar(0, 0, 0, 0));
    EnviarImagenDemo(cuadros);
}
}

//-------------------------------------------------------------------------------------------------