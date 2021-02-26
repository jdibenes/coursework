//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Solucionador de sudoku X38
// Juan Carlos Dibene Simental
// 11210637
//-------------------------------------------------------------------------------------------------
// ocr.cpp 
// Reconocimiento de caracteres utilizando tesseract 3.02 (http://code.google.com/p/tesseract-ocr/)
//-------------------------------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS

#include <tesseract/baseapi.h>
#include <new.h>
#include "ocr.h"

namespace x38ocr
{
tesseract::TessBaseAPI *g_ocr = NULL;

bool Init()
{
	if (g_ocr != NULL) {return true;}
	g_ocr = new(std::nothrow) tesseract::TessBaseAPI();
	if (g_ocr == NULL) {return false;}
	if (g_ocr->Init(NULL, "eng") == 0) {return true;}
	delete g_ocr;
	g_ocr = NULL;
	return false;
}

char LeerCaracter(unsigned char const *datos, int cols, int rows)
{
	if (g_ocr == NULL) {return '\0';}
    g_ocr->SetImage(datos, cols, rows, 1, cols);
	char *txt = g_ocr->GetUTF8Text();
	if (txt == NULL) {return '\0';}
	char caracter = txt[0];
    g_ocr->Clear();
	delete [] txt;
	return caracter;
}

void Close()
{
	if (g_ocr == NULL) {return;}
	g_ocr->End();
	delete g_ocr;
	g_ocr = NULL;
}
}

//-------------------------------------------------------------------------------------------------