//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// profiler.h
// Pequeño profiler para medir el desempeño del programa
//-------------------------------------------------------------------------------------------------

#pragma once

namespace x38profiler
{
void Init();
double ObtenerFrecuencia();
void MarcarInicio();
double MarcarFinal();
}

//-------------------------------------------------------------------------------------------------