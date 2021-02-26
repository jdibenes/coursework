//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// profiler.h
// Peque�o profiler para medir el desempe�o del programa
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