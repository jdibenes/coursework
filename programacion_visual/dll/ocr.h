//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// ocr.h
// Reconocimiento de caracteres utilizando tesseract 3.02 (http://code.google.com/p/tesseract-ocr/)
// Incluir x38ocr.h en lugar de este archivo en otros proyectos
//-------------------------------------------------------------------------------------------------

#pragma once

namespace x38ocr
{
// Inicializa tesseract. Debe llamarse antes de utilizar la funcion LeerCaracter.
__declspec(dllexport) bool Init();

// Lee el caracter contenido en una imagen almacenada en un Mat de OpenCV.
// La imagen debe estar en formato grayscale.
__declspec(dllexport) char LeerCaracter(unsigned char const *datos, int cols, int rows);

// Libera todos los recursos utilizados por tesseract.
__declspec(dllexport) void Close();
}

//-------------------------------------------------------------------------------------------------