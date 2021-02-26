//-------------------------------------------------------------------------------------------------
// Programacion Visual
// Proyecto OpenCV
// Juan Carlos Dibene Simental
// Solucionador de sudoku X38
// 11210637
//-------------------------------------------------------------------------------------------------
// sudoku.cpp
// Solucionador de sudoku utilizando most constrained first heuristic
// Teoria: http://www.sysexpand.com/?path=exercises/sudoku-solver
//-------------------------------------------------------------------------------------------------

#include <vector>
#include "sudoku.h"

namespace x38sudoku
{
char            (*g_sudoku)[9];
std::vector<char> g_candidatos[9][9];
bool              g_estado[9][9];
std::vector<int>  g_espacio;

int PackIndice(int i, int j)
{
    return (i * 9) + j;
}

bool Init()
{
    g_espacio.reserve(81);
    g_espacio.clear();

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            g_candidatos[i][j].reserve(9);
            g_estado[i][j] = false;
            if (g_sudoku[i][j] < '1' || g_sudoku[i][j] > '9') {g_espacio.push_back(PackIndice(i, j));}
        }
    }

    return g_espacio.size() > 0;
}

void ActualizarCandidatos(int i, int j)
{
    int io = i - (i % 3);
    int jo = j - (j % 3);
    int k;

    g_candidatos[i][j].clear();    

    for (char c = '1'; c <= '9'; c++)
    {
        for (k = 0; k < 9 && g_sudoku[i][k] != c && g_sudoku[k][j] != c && g_sudoku[io + (k / 3)][jo + (k % 3)] != c; k++);
        if (k >= 9) {g_candidatos[i][j].push_back(c);}
    }

    g_estado[i][j] = true;
}

std::vector<char> const *ObtenerCandidatos(int i, int j)
{
    if (!g_estado[i][j]) {ActualizarCandidatos(i, j);}
    return &g_candidatos[i][j];
}

void Invalidar(int i, int j)
{
    int io = i - (i % 3);
    int jo = j - (j % 3);
    for (int k = 0; k < 9; k++) {g_estado[i][k] = g_estado[k][j] = g_estado[io + (k / 3)][jo + (k % 3)] = false;}
}

void ObtenerEspacios(std::vector<int>::iterator &inicio, std::vector<int>::iterator &fin)
{
    inicio = g_espacio.begin();
    fin    = g_espacio.end();
}

void UnpackIndice(int indice, int &i, int &j)
{
    i = indice / 9;
    j = indice % 9;
}

bool Verificar()
{
    int n[3][9];
    int k[3];
    int io;
    int jo;

    int c = 0;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (g_sudoku[i][j] >= '1' && g_sudoku[i][j] <= '9') {++c;}
        }
    }

    if (c < 17) {return false;}

    for (int i = 0; i < 9; i++)
    {
        memset(n, 0, sizeof(n));

        io = (i / 3) * 3;
        jo = (i % 3) * 3;

        for (int j = 0; j < 9; j++)
        {
            k[0] = g_sudoku[i][j];
            if (k[0] >= '1' && k[0] <= '9') {if (n[0][k[0] - '1'] != 0) {return false;} else {n[0][k[0] - '1'] = 1;}}

            k[1] = g_sudoku[j][i];
            if (k[1] >= '1' && k[1] <= '9') {if (n[1][k[1] - '1'] != 0) {return false;} else {n[1][k[1] - '1'] = 1;}}

            k[2] = g_sudoku[io + (j / 3)][jo + (j % 3)];
            if (k[2] >= '1' && k[2] <= '9') {if (n[2][k[2] - '1'] != 0) {return false;} else {n[2][k[2] - '1'] = 1;}}
        }
    }

    return true;
}

bool Resolvedor()
{
    const int ocupado = -1;

    std::vector<char> const *candidatos[2];

    std::vector<int>::iterator it;
    std::vector<int>::iterator fin;
    std::vector<int>::iterator sel;

    int fil;
    int col;
    int i;
    int j;
    int espacio;

    ObtenerEspacios(it, fin);

    candidatos[0] = NULL;
    candidatos[1] = NULL;

    for (; it != fin; it++)
    {
        if (*it != ocupado)
        {
            UnpackIndice(*it, i, j);
            candidatos[1] = ObtenerCandidatos(i, j);
            if (candidatos[0] == NULL || candidatos[1]->size() < candidatos[0]->size())
            {
                candidatos[0] = candidatos[1];
                fil = i;
                col = j;                
                sel = it;
            }
        }
    }

    if (candidatos[0] == NULL) {return true;}

    espacio = *sel;

    for (size_t k = 0; k < candidatos[0]->size(); k++)
    {
        g_sudoku[fil][col] = candidatos[0]->at(k);
        *sel = ocupado;
        Invalidar(fil, col);
        if (Resolvedor()) {return true;}
        g_sudoku[fil][col] = '0';
        *sel = espacio;
        Invalidar(fil, col);
    }

    return false;
}

bool Resolver(char sudoku[9][9])
{
    g_sudoku = sudoku;
    if (!Verificar() || !Init()) {return false;}
    return Resolvedor();
}
}

//-------------------------------------------------------------------------------------------------