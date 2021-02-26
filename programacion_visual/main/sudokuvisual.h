//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// sudokuvisual.h
// Resuelve un sudoku utilizando OpenCV (http://opencv.org/)
//-------------------------------------------------------------------------------------------------

#pragma once

#include <opencv2/opencv.hpp>

namespace x38sudokuvisual
{
bool Init();
bool Resolver(cv::Mat const &imagen, cv::Mat &resultado, cv::Rect *roirc = NULL);
bool VerificarEstable();
void Close();
void Demo(cv::Mat const &imagen);
}

//-------------------------------------------------------------------------------------------------