//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// profiler.cpp
// Pequeño profiler para medir el desempeño del programa
//-------------------------------------------------------------------------------------------------

#include <Windows.h>

namespace x38profiler
{
LARGE_INTEGER g_div[2];
double g_frec;

void Init()
{
    LARGE_INTEGER frec;
    QueryPerformanceFrequency(&frec);
    g_frec = (double)frec.QuadPart;
}

double ObtenerFrecuencia()
{
    return g_frec;
}

void MarcarInicio()
{
    QueryPerformanceCounter(&g_div[0]);
}

double MarcarFinal()
{
    QueryPerformanceCounter(&g_div[1]);
    return (double)(g_div[1].QuadPart - g_div[0].QuadPart);
}
}

//-------------------------------------------------------------------------------------------------