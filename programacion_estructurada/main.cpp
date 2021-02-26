
#define _WIN32_WINNT 0x0400

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

using namespace std;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef
struct
{
    u16 a;   // año
    u16 m;   // mes
    u16 d;   // dia
    u16 ds;  // dia de la semana
    u16 hr;  // hora
    u16 min; // minutos
    u16 seg; // segundos
    u16 ms;  // milisegundos
}
FECHA;

static char const * const Dia[7] =
{
    "Domingo",
    "Lunes",
    "Martes",
    "Miercoles",
    "Jueves",
    "Viernes",
    "Sabado"
};

static char const * const Mes[12] =
{
    "Enero",
    "Febrero",
    "Marzo",
    "Abril",
    "Mayo",
    "Junio",
    "Julio",
    "Agosto",
    "Septiembre",
    "Octubre",
    "Noviembre",
    "Diciembre"
};

static COORD const PosInicial =
{
    0,
    0
};

void saec_EjecutarContris();

static HANDLE hStdOut, hStdIn;
static CONSOLE_SCREEN_BUFFER_INFOEX StdOutInfo;

void saec_ObtenerFecha(FECHA *fecha)
{
    SYSTEMTIME st;

    GetLocalTime(&st);

    fecha->a = st.wYear;
    fecha->m = st.wMonth;
    fecha->d = st.wDay;
    fecha->ds = st.wDayOfWeek;
    fecha->hr = st.wHour;
    fecha->min = st.wMinute;
    fecha->seg = st.wSecond;
    fecha->ms = st.wMilliseconds;
}

void saec_IniciaCon()
{
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleScreenBufferInfoEx(hStdOut, &StdOutInfo);
}

void saec_SalirCon()
{
    SetConsoleScreenBufferInfoEx(hStdOut, &StdOutInfo);
    hStdOut = hStdIn = INVALID_HANDLE_VALUE;
}

void saec_LimpiarPantalla()
{
    DWORD num, total;
    CONSOLE_SCREEN_BUFFER_INFO stdoutinfo;

    GetConsoleScreenBufferInfo(hStdOut, &stdoutinfo);
    num = stdoutinfo.dwSize.X * stdoutinfo.dwSize.Y;

    SetConsoleCursorPosition(hStdOut, PosInicial);
    FillConsoleOutputAttribute(hStdOut, StdOutInfo.wAttributes, num, PosInicial, &total);
    FillConsoleOutputCharacter(hStdOut, ' ', num, PosInicial, &total);
}

void saec_AjustarCin()
{
    cin.clear();
    cin.sync();
}

void saec_RegistrarAsistencia()
{
    ofstream lista;
    char numero[20], ch, archivo[20], hora[12];
    bool invdato;
    FECHA fecha;

    do
    {
        saec_LimpiarPantalla();
        saec_ObtenerFecha(&fecha);

        cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-= Registro de asistencia =-=-=-=-=-=-=-=-=-=-=-=-=-=" << "\n";        
        printf("%02d/%02d/%04d\n", fecha.d, fecha.m, fecha.a);
        sprintf(hora, "%02d:%02d:%02d", fecha.hr, fecha.min, fecha.seg);
        printf("%s\n", hora);

        cout << "\n" << "> Numero de control: ";
        cin.getline(numero, 9, '\n');
        saec_AjustarCin();

        numero[8] = '\0';
        invdato = false;

        for (int x = 0; x < 8; x++)
        {
            if (numero[x] >= '0' && numero[x] <= '9') {continue;}
            invdato = true;
            cout << numero << " no es un numero de control valido" << "\n";
            break;
        }

        if (!invdato)
        {
            sprintf(archivo, ".\\%d-%d-%d.txt", fecha.a, fecha.m, fecha.d);
            lista.open(archivo, ios::app);        

            if (lista.is_open())
            {
                lista << numero << " " << hora << "\n";
                cout << "Entrada registrada satisfactoriamente" << "\n";
                lista.close();
            }
            else
            {
                cout << "Error al abrir el archivo \"" << archivo << "\"" << "\n";
            }
        }

        cout << "\n" << "Agregar otro (s/n): ";
        cin >> ch;
        saec_AjustarCin();
    }
    while ((ch & ~32) == 'S');
}

void saec_VerAsistencias()
{
    ifstream lista;
    char fecha[20], dia[4], mes[4], a[8], ch, archivo[20], numero[12], hora[12];
    bool invdato;

    do
    {
        saec_LimpiarPantalla();

        cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-= Visor de registros =-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << "\n";
        cout << "Introduzca la fecha del registro que desea ver ";
        cout << "utilizando el siguiente formato" << "\n" << "dd-mm-aaaa" << "\n";
        cout << "(d = dia; m = mes; a = a\xA4o; Ejemplo: 22-04-2011)" << "\n";

        cout << "\n" << "Fecha: ";
        cin.getline(fecha, 11, '\n');
        saec_AjustarCin();

        fecha[10] = '\0';
        invdato = false;

        for (int x = 0; x < 10; x++)
        {
            if ((fecha[x] >= '0' && fecha[x] <= '9') || x == 2 || x == 5) {continue;}     
            invdato = true;
            cout << "La fecha especificada no tiene un formato valido" << "\n" << "\n";
            break;
        }

        if (!invdato)
        {
            a[0] = fecha[6];
            a[1] = fecha[7];
            a[2] = fecha[8];
            a[3] = fecha[9];
            a[4] = '\0';

            mes[0] = fecha[3];
            mes[1] = fecha[4];
            mes[2] = '\0';

            dia[0] = fecha[0];
            dia[1] = fecha[1];
            dia[2] = '\0';

            sprintf(archivo, ".\\%s-%s-%s.txt", a, mes, dia);
            lista.open(archivo);

            if (lista.is_open())
            {
                cout << "\n" << "Asistencias registradas el dia " << fecha << "\n";
                cout << "No. Control Hora" << "\n";

                while (!lista.eof())
                {
                    lista >> numero >> hora;
                    cout << numero << "    " << hora << "\n";
                }

                lista.close();
            }
            else
            {
                cout << "\n" << "Error al abrir el registro \"" << archivo << "\"" << "\n" << "\n";
            }
        }

        cout << "Ver otro registro (s/n): ";
        cin >> ch;
        saec_AjustarCin();
    }
    while ((ch & ~32) == 'S');
}

int main()
{
    FECHA f;
    char op;

    saec_IniciaCon();
    SetConsoleTitle("SAEC Version 0.1a - Juan Carlos Dibene Simental [11210637]");

    do
    {
        saec_LimpiarPantalla();
        saec_ObtenerFecha(&f);        

        cout << "=-=-=-=-=-=-=-= Simple lista de asistencia en consola ( SAEC ) =-=-=-=-=-=-=-=" << "\n";
        cout << "Hoy es " << Dia[f.ds] << " " << f.d << " de " << Mes[f.m - 1] << " de " << f.a << "\n";
        cout << "\n" << "Opciones: " << "\n";
        cout << "1. Registrar asistencias del dia" << "\n";
        cout << "2. Ver asistencias" << "\n";
        cout << "3. Perder tiempo" << "\n";
        cout << "0. Salir" << "\n";
        cout << "\n";

        cout << "Opcion (0 - 3): ";
        cin >> op;
        saec_AjustarCin();

        switch (op)
        {
        case '0':
            break;
        case '1':
            saec_RegistrarAsistencia();
            break;
        case '2':
            saec_VerAsistencias();
            break;
        case '3':
            saec_EjecutarContris();
            break;
        }
    }
    while (op != '0');

    saec_SalirCon();
    return 0;
}

//-------------------------------------------------------------------------------------------------
// contris -  clon de ese juego viejo de hacer lineas con los bloques que caen
// Juan Carlos Dibene Simental - 2011
//-------------------------------------------------------------------------------------------------
typedef
struct
{
    // Bloque de datos con el siguiente formato:
    // data[2][tr][tc]
    // data[0] guarda tr * tc caracteres ascii
    // data[1] guarda la informacion de color de cada caracter
    char const *data;
    s16 tr; // Total de renglones
    s16 tc; // Total de columnas
    // Coordenadas del punto superior izquierdo dentro del bloque
    s16 ox; // Izquierda
    s16 oy; // Arriba
    // Coordenadas del rectangulo a dibujar en la consola
    s16 x0; // Izquierda
    s16 y0; // Arriba
    s16 x1; // Derecha
    s16 y1; // Abajo
}
BLOQUE;

typedef
struct
{
    u8 id;   // Tipo de figura
    u8 anim; // Cuadro de animacion
    // Coordenadas del punto superior izquierdo del mosaico
    u8 x;    // Columna
    u8 y;    // Fila
}
Mosaico;

#define TECLAPRESIONADA(_vk) (GetAsyncKeyState(_vk) & 32768)

static char const Figura[7][4][2][4][4] =
{
    { // Cuadrado
    {{{'#','#',' ',' '},{'#','#',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 25, 25,  0,  0},{ 25, 25,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'#','#',' ',' '},{'#','#',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 25, 25,  0,  0},{ 25, 25,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'#','#',' ',' '},{'#','#',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 25, 25,  0,  0},{ 25, 25,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'#','#',' ',' '},{'#','#',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 25, 25,  0,  0},{ 25, 25,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}}
    },
    { // Triangulo
    {{{' ','@',' ',' '},{'@','@','@',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{  0, 42,  0,  0},{ 42, 42, 42,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'@',' ',' ',' '},{'@','@',' ',' '},{'@',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 42,  0,  0,  0},{ 42, 42,  0,  0},{ 42,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'@','@','@',' '},{' ','@',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 42, 42, 42,  0},{  0, 42,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{' ','@',' ',' '},{'@','@',' ',' '},{' ','@',' ',' '},{' ',' ',' ',' '}},
     {{  0, 42,  0,  0},{ 42, 42,  0,  0},{  0, 42,  0,  0},{  0,  0,  0,  0}}},
    },
    { // Escalon
    {{{' ','$','$',' '},{'$','$',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{  0, 76, 76,  0},{ 76, 76,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'$',' ',' ',' '},{'$','$',' ',' '},{' ','$',' ',' '},{' ',' ',' ',' '}},
     {{ 76,  0,  0,  0},{ 76, 76,  0,  0},{  0, 76,  0,  0},{  0,  0,  0,  0}}},
    {{{' ','$','$',' '},{'$','$',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{  0, 76, 76,  0},{ 76, 76,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'$',' ',' ',' '},{'$','$',' ',' '},{' ','$',' ',' '},{' ',' ',' ',' '}},
     {{ 76,  0,  0,  0},{ 76, 76,  0,  0},{  0, 76,  0,  0},{  0,  0,  0,  0}}},
    },
    { // Escalon invertido
    {{{'%','%',' ',' '},{' ','%','%',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 59, 59,  0,  0},{  0, 59, 59,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{' ','%',' ',' '},{'%','%',' ',' '},{'%',' ',' ',' '},{' ',' ',' ',' '}},
     {{  0, 59,  0,  0},{ 59, 59,  0,  0},{ 59,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'%','%',' ',' '},{' ','%','%',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 59, 59,  0,  0},{  0, 59, 59,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{' ','%',' ',' '},{'%','%',' ',' '},{'%',' ',' ',' '},{' ',' ',' ',' '}},
     {{  0, 59,  0,  0},{ 59, 59,  0,  0},{ 59,  0,  0,  0},{  0,  0,  0,  0}}},
    },
    { // L
    {{{'^',' ',' ',' '},{'^',' ',' ',' '},{'^','^',' ',' '},{' ',' ',' ',' '}},
     {{ 93,  0,  0,  0},{ 93,  0,  0,  0},{ 93, 93,  0,  0},{  0,  0,  0,  0}}},
    {{{'^','^','^',' '},{'^',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{ 93, 93, 93,  0},{ 93,  0,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'^','^',' ',' '},{' ','^',' ',' '},{' ','^',' ',' '},{' ',' ',' ',' '}},
     {{ 93, 93,  0,  0},{  0, 93,  0,  0},{  0, 93,  0,  0},{  0,  0,  0,  0}}},
    {{{' ',' ','^',' '},{'^','^','^',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{  0,  0, 93,  0},{ 93, 93, 93,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    },
    { // L invertida
    {{{' ','&',' ',' '},{' ','&',' ',' '},{'&','&',' ',' '},{' ',' ',' ',' '}},
     {{  0,110,  0,  0},{  0,110,  0,  0},{110,110,  0,  0},{  0,  0,  0,  0}}},
    {{{'&',' ',' ',' '},{'&','&','&',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{110,  0,  0,  0},{110,110,110,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'&','&',' ',' '},{'&',' ',' ',' '},{'&',' ',' ',' '},{' ',' ',' ',' '}},
     {{110,110,  0,  0},{110,  0,  0,  0},{110,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'&','&','&',' '},{' ',' ','&',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{110,110,110,  0},{  0,  0,110,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    },
    { // Barra
    {{{'*',' ',' ',' '},{'*',' ',' ',' '},{'*',' ',' ',' '},{'*',' ',' ',' '}},
     {{127,  0,  0,  0},{127,  0,  0,  0},{127,  0,  0,  0},{127,  0,  0,  0}}},
    {{{'*','*','*','*'},{' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{127,127,127,127},{  0,  0,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    {{{'*',' ',' ',' '},{'*',' ',' ',' '},{'*',' ',' ',' '},{'*',' ',' ',' '}},
     {{127,  0,  0,  0},{127,  0,  0,  0},{127,  0,  0,  0},{127,  0,  0,  0}}},
    {{{'*','*','*','*'},{' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}},
     {{127,127,127,127},{  0,  0,  0,  0},{  0,  0,  0,  0},{  0,  0,  0,  0}}},
    }
};

static char const Dimension[2][7][4] =
{
    { // Largo
    {2, 2, 2, 2}, // Cuadrado
    {3, 2, 3, 2}, // Triangulo
    {3, 2, 3, 2}, // Escalon
    {3, 2, 3, 2}, // Escalon invertido
    {2, 3, 2, 3}, // L
    {2, 3, 2, 3}, // L invertida
    {1, 4, 1, 4}  // Barra
    },
    { // Alto
    {2, 2, 2, 2}, // Cuadrado
    {2, 3, 2, 3}, // Triangulo
    {2, 3, 2, 3}, // Escalon
    {2, 3, 2, 3}, // Escalon invertido
    {3, 2, 3, 2}, // L
    {3, 2, 3, 2}, // L invertida
    {4, 1, 4, 1}  // Barra
    }
};

static char Fondo[2][24][50];
static Mosaico Objeto;
static int Lineas;

void saec_EscribirBloque(BLOQUE const *b)
{
    CHAR_INFO *cuadrobuffer;
    int dx, dy, bc, bd, bl, x, y;
    COORD dimension, origen;
    SMALL_RECT rect;
    
    dx = 1 + b->x1 - b->x0;
    dy = 1 + b->y1 - b->y0;

    cuadrobuffer = (CHAR_INFO *)malloc(dx * dy * sizeof(CHAR_INFO));
    if (!cuadrobuffer) {return;}    
    
    bl = (b->tc * b->tr);

    for (x = 0; x < dy; x++)
    {
        for (y = 0; y < dx; y++)
        {
            bc = (x * dx) + y;
            bd = b->ox + y + ((b->oy + x) * b->tc);
            cuadrobuffer[bc].Char.AsciiChar = b->data[bd];
            cuadrobuffer[bc].Attributes = b->data[bl + bd];
        }
    }

    dimension.X = dx;
    dimension.Y = dy;

    origen.X = 0;
    origen.Y = 0;

    rect.Top = b->y0;
    rect.Left = b->x0;
    rect.Bottom = b->y1;
    rect.Right = b->x1;

    WriteConsoleOutput(hStdOut, cuadrobuffer, dimension, origen, &rect);
    free(cuadrobuffer);
}

void saec_PresentarCuadro()
{
    BLOQUE b;

    b.data = &Fondo[0][0][0];
    b.ox = 0;
    b.oy = 0;
    b.tr = 24;
    b.tc = 50;
    b.x0 = 0;
    b.x1 = 49;
    b.y0 = 0;
    b.y1 = 23;

    saec_EscribirBloque(&b);
}

void saec_LimpiarCuadro(s8 x0, s8 x1, s8 y0, s8 y1)
{
    for (int y = y0; y <= y1; y++)
    {
        for (int x = x0; x <= x1; x++)
        {
            Fondo[0][y][x] = ' ';
            Fondo[1][y][x] = 0;
        }
    }
}

void saec_MosaicoBorrarFondo()
{
    int i, a, v, h;

    i = Objeto.id;
    a = Objeto.anim;
    v = Objeto.y;
    h = Objeto.x;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (Figura[i][a][0][y][x] != ' ')
            {
                Fondo[0][v + y][h + x] = ' ';
                Fondo[1][v + y][h + x] = 0;
            }
        }
    }
}

void saec_BlitMosaico()
{
    int i, a, v, h;

    i = Objeto.id;
    a = Objeto.anim;
    v = Objeto.y;
    h = Objeto.x;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (Figura[i][a][0][y][x] != ' ')
            {
                Fondo[0][v + y][h + x] = Figura[i][a][0][y][x];
                Fondo[1][v + y][h + x] = Figura[i][a][1][y][x];
            }
        }
    }
}

bool saec_MoverMosaicoX(bool derecha)
{
    int i, a, v, h, nx;

    i = Objeto.id;
    a = Objeto.anim;
    v = Objeto.y;
    h = Objeto.x;    

    if (!derecha)
    {
        if (!h) {return true;}
        nx = h - 1;
    }
    else
    {
        nx = h + 1;
        if (nx > (16 - Dimension[0][i][a])) {return true;}
    }

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (Figura[i][a][0][y][x] != ' ' && Fondo[0][v + y][nx + x] != ' ') {return true;}
        }
    }

    Objeto.x = nx;
    return false;
}

bool saec_MoverMosaicoY()
{
    int i, a, v, h, ny;

    i = Objeto.id;
    a = Objeto.anim;
    v = Objeto.y;
    h = Objeto.x;

    if (v + Dimension[1][i][a] == 24) {return true;}
    ny = v + 1;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (Figura[i][a][0][y][x] != ' ' && Fondo[0][ny + y][h + x] != ' ') {return true;}
        }
    }

    Objeto.y = ny;
    return false;
}

bool saec_ReiniciarMosaico()
{
    Objeto.id = rand() % 7;
    Objeto.anim = 0;
    Objeto.x = 6;
    Objeto.y = 0;

    if (saec_MoverMosaicoY()) {return false;}
    Objeto.y = 0;

    Objeto.x = 7;
    if (saec_MoverMosaicoX(false)) {return false;}

    Objeto.x = 5;
    if (saec_MoverMosaicoX(true)) {return false;}

    Objeto.x = 6;
    Objeto.y = 0;
    return true;
}

void saec_ColocarCursor(s16 x, s16 y)
{
    COORD npos;

    npos.X = x;
    npos.Y = y;

    SetConsoleCursorPosition(hStdOut, npos);
}

void saec_IniciarContris()
{
    saec_LimpiarPantalla();
    saec_ColocarCursor(29, 1);
    saec_LimpiarCuadro(0, 49, 0, 23);

    for (int y = 0; y < 24; y++)
    {
        Fondo[0][y][16] = '|';
        Fondo[1][y][16] = 15;
    }

    memcpy(&Fondo[0][0][18], "Contris", 7);
    Fondo[1][0][18] = 25;
    Fondo[1][0][19] = 42;
    Fondo[1][0][20] = 76;
    Fondo[1][0][21] = 59;
    Fondo[1][0][22] = 93;
    Fondo[1][0][23] = 110;
    Fondo[1][0][24] = 127;

    memcpy(&Fondo[0][1][18], "Lineas: 0000", 12);
    memset(&Fondo[1][1][18], 7, 12);
    memcpy(&Fondo[0][3][18], "Controles", 9);
    memset(&Fondo[1][3][18], 15, 9);
    memcpy(&Fondo[0][4][18], "X: Rotar a la derecha", 21);
    memset(&Fondo[1][4][18], 7, 21);
    memcpy(&Fondo[0][5][18], "Z: Rotar a la izquierda", 23);
    memset(&Fondo[1][5][18], 7, 23);
    memcpy(&Fondo[0][6][18], "Flechas: Mover", 14);
    memset(&Fondo[1][6][18], 7, 14);
    memcpy(&Fondo[0][7][18], "Enter: Pausa", 12);
    memset(&Fondo[1][7][18], 7, 12);
    memcpy(&Fondo[0][8][18], "Retroceso: Reiniciar", 20);
    memset(&Fondo[1][8][18], 7, 20);
    memcpy(&Fondo[0][9][18], "Esc: Salir", 10);
    memset(&Fondo[1][9][18], 7, 10);
    memcpy(&Fondo[0][11][18], "Hora", 4);
    memset(&Fondo[1][11][18], 7, 4);
    memset(&Fondo[1][12][18], 7, 8);
    memcpy(&Fondo[0][22][18], "jcds", 4);
    memset(&Fondo[1][22][18], 15, 4);
    memcpy(&Fondo[0][23][18], "v0.1a", 5);
    memset(&Fondo[1][23][18], 15, 5);

    srand((unsigned int)time(0));
    Lineas = 0;
    saec_ReiniciarMosaico();
    saec_BlitMosaico();
}

void saec_SalirContris()
{
    saec_LimpiarPantalla();
    
    std::cout << "\xADGracias por jugar!" << "\n";
    std::cout << "Lineas: " << Lineas << "\n";
    std::cout << "Presione enter para continuar..." << "\n";
    FlushConsoleInputBuffer(hStdIn);
    std::cin.get();
    saec_AjustarCin();
}

void saec_RevisarLineas()
{
    int base, x, y, z, w;

    for (y = 23; y >= 0; y--)
    {
        for (x = 0; x < 16; x++) {if (Fondo[0][y][x] == ' ') {break;}}
        if (x == 16)
        {
            for (z = y; z > 0; z--)
            {
                memcpy(&Fondo[0][z][0], &Fondo[0][z - 1][0], 16);
                memcpy(&Fondo[1][z][0], &Fondo[1][z - 1][0], 16);
            }

            for (w = 0; w < 4; w++)
            {
                base = 29 - w;
                if (Fondo[0][1][base] != '9')
                {
                    Fondo[0][1][base]++;
                    break;
                }
                else
                {
                    Fondo[0][1][base] = '0';
                }
            }

            y++;
            Lineas++;

            memset(&Fondo[0][0][0], ' ', 16);
            memset(&Fondo[1][0][0], 0, 16);
        }
    }
}

void saec_RotarMosaico(bool derecha)
{
    int i, a, v, h, na;

    i = Objeto.id;
    a = Objeto.anim;
    v = Objeto.y;
    h = Objeto.x;

    na = (a + (derecha ? 1 : -1)) & 3;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (Figura[i][na][0][y][x] != ' ' && Fondo[0][v + y][h + x] != ' ') {return;}
        }
    }

    Objeto.anim = na;
}

void saec_EnPausa()
{    
    while (TECLAPRESIONADA(VK_RETURN));

    memcpy(&Fondo[0][0][26], "Pausa", 5);
    memset(&Fondo[1][0][26], 15, 5);

    saec_PresentarCuadro();

    while (!TECLAPRESIONADA(VK_RETURN));

    memset(&Fondo[0][0][26], ' ', 5);
    memset(&Fondo[1][0][26], 15, 5);
    Sleep(100);
}

bool saec_EnPerder()
{
    saec_BlitMosaico();

    memcpy(&Fondo[0][2][3], "Ha perdido", 10);
    memset(&Fondo[1][2][3], 31, 10);

    memcpy(&Fondo[0][4][2], "Presione Esc", 12);
    memset(&Fondo[1][4][2], 47, 12);
    memcpy(&Fondo[0][5][3], "para salir", 10);
    memset(&Fondo[1][5][3], 47, 10);

    memcpy(&Fondo[0][7][1], "Retroceso para", 14);
    memset(&Fondo[1][7][1], 79, 14);
    memcpy(&Fondo[0][8][3], "Reiniciar", 9);
    memset(&Fondo[1][8][3], 79, 9);

    saec_PresentarCuadro();

    for (;;)
    {
        Sleep(1);
        if (TECLAPRESIONADA(VK_ESCAPE)) {return false;}
        if (TECLAPRESIONADA(VK_BACK)) {return true;}
    }
}

void saec_EjecutarContris()
{
    bool ycolision;
    int cuadro, basecuadro;
    FECHA fecha;
    char hora[12];

_reiniciar:    
    saec_IniciarContris();

    cuadro = 0;
    basecuadro = 6;

    while (!TECLAPRESIONADA(VK_ESCAPE))
    {
        saec_ObtenerFecha(&fecha);
        sprintf(hora, "%02d:%02d:%02d", fecha.hr, fecha.min, fecha.seg);
        memcpy(&Fondo[0][12][18], hora, 8);

        saec_PresentarCuadro();
        Sleep(100);
        saec_MosaicoBorrarFondo();

        ycolision = false;

        if (TECLAPRESIONADA(VK_LEFT)) {saec_MoverMosaicoX(false);}
        else if (TECLAPRESIONADA(VK_RIGHT)) {saec_MoverMosaicoX(true);}
        else if (TECLAPRESIONADA(VK_DOWN)) {ycolision = saec_MoverMosaicoY();}
        else if (TECLAPRESIONADA(0x5A)) {saec_RotarMosaico(false);}
        else if (TECLAPRESIONADA(0x58)) {saec_RotarMosaico(true);}
        else if (TECLAPRESIONADA(VK_RETURN)) {saec_EnPausa();}
        else if (TECLAPRESIONADA(VK_BACK)) {goto _reiniciar;}

        if (cuadro >= basecuadro) 
        {
            if (!ycolision) {ycolision = saec_MoverMosaicoY();}
            cuadro = 0;
        }

        if (Lineas < 5) {basecuadro = 5;}
        else if (Lineas < 12) {basecuadro = 4;}
        else if (Lineas < 21) {basecuadro = 3;}
        else if (Lineas < 32) {basecuadro = 2;}
        else if (Lineas < 45) {basecuadro = 1;}
        else {basecuadro = 0;}

        saec_BlitMosaico();

        if (ycolision)
        {
            saec_RevisarLineas();
            if (!saec_ReiniciarMosaico())
            {
                if (saec_EnPerder()) {goto _reiniciar;} else {break;}
            }
        }

        cuadro++;
    }

    saec_SalirContris();
}
