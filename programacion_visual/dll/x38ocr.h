//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// x38ocr.h
// Reconocimiento de caracteres utilizando tesseract 3.02 (http://code.google.com/p/tesseract-ocr/)
//-------------------------------------------------------------------------------------------------

#pragma once

namespace x38ocr
{
// Inicializa tesseract. Debe llamarse antes de utilizar la funcion LeerCaracter.
__declspec(dllimport) bool Init();

// Lee el caracter contenido en una imagen almacenada en un Mat de OpenCV.
// La imagen debe estar en formato grayscale.
__declspec(dllimport) char LeerCaracter(unsigned char const *datos, int cols, int rows);

// Libera todos los recursos utilizados por tesseract.
__declspec(dllimport) void Close();
}

//-------------------------------------------------------------------------------------------------