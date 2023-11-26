#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <gdiplus.h>
#include <windowsx.h>

int siseCase = 100;
int initX = 50;
int initY = 50;
int sizePion = 10;

//jeu
char board[7][11] =
{
    {'.','.','.','.','.','.','.','.','.','.','.'},
    {'.','o','o','o','o','o','o','o','o','o','.'},
    {'.','o','o','o','O','O','O','o','o','o','.'},
    {'.','o','x','o','X','w','O','x','o','x','.'},
    {'.','x','x','x','X','X','X','x','x','x','.'},
    {'.','x','x','x','x','x','x','x','x','x','.'},
    {'.','.','.','.','.','.','.','.','.','.','.'}
};
char boardInit[7][11] =
{
    {'.','.','.','.','.','.','.','.','.','.','.'},
    {'.','o','o','o','o','o','o','o','o','o','.'},
    {'.','o','o','o','O','O','O','o','o','o','.'},
    {'.','o','x','o','X','w','O','x','o','x','.'},
    {'.','x','x','x','X','X','X','x','x','x','.'},
    {'.','x','x','x','x','x','x','x','x','x','.'},
    {'.','.','.','.','.','.','.','.','.','.','.'}
};
char boardKill[7][11] =
{
    {'.','.','.','.','.','.','.','.','.','.','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','.','.','.','.','.','.','.','.','.','.'}
};
char boardMove[7][11] =
{
    {'.','.','.','.','.','.','.','.','.','.','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','w','w','w','w','w','w','w','w','w','.'},
    {'.','.','.','.','.','.','.','.','.','.','.'}
};

int oldPion_L  = 0;
int oldPion_C  = 0;
int selectedPion_L = 0;
int selectedPion_C = 0;
int numCanEat = 0;
int killed = 0;
int oldPosChooseL = 0;
int oldPosChooseC = 0;
int msgScreen = 0;
bool mustChoose = false;
int playerOn = 0;
bool isCanReKill = false;

const char phrase[5][30] =
{
    "Déplacer un pion",
    "C'est au tour du rouge",
    "C'est au tour du bleu",
    "Le joueur rouge a gagné",
    " Le joueur bleu a gagné "
};

int number_move(int x, int y)
{
    int mov = 0;
    if(x%2 != y%2)
    {
        for(int i = -1 ; i < 2; i++)
        {
            if(board[x-i][y] == 'w') mov++;
        }
        for(int j = -1 ; j < 2; j++)
        {
            if(board[x][y-j] == 'w') mov++;
        }
    }
    else
    {
        for(int i = -1 ; i < 2; i++)
        {
            for(int j = -1 ; j < 2; j++)
            {
                if(board[x-i][y-j] == 'w') mov++;
            }
        }
    }

    return mov;
}

int isCanChoose(int sl, int sc)
{
    int num = 0;
    char aim = board[sl][sc] == 'x' ? 'o' : 'x';

    // H sur 2-8 C
    if(sc != 1 && sc != 9)
    {
        if(oldPion_L == sl)
        {
            // o-+-x-o
            if(board[sl][sc + 1] == aim && board[sl][sc - 2] == aim)
            {
                boardKill[sl][sc + 1] = '+';
                boardKill[sl][sc - 2] = '+';
                num++;
            }

            // o-x-+-o
            if(board[sl][sc - 1] == aim && board[sl][sc + 2] == aim)
            {
                boardKill[sl][sc - 1] = '+';
                boardKill[sl][sc + 2] = '+';
                num++;
            }
        }

    }

    //V sur 2-4 L
    if(sl != 1 && sl != 5)
    {
        if(oldPion_C == sc)
        {
            // To-x-+-oB
            if(board[sl - 1][sc] == aim && board[sl + 2][sc] == aim)
            {
                boardKill[sl - 1][sc] = '+';
                boardKill[sl + 2][sc] = '+';
                num++;
            }
            // To-+-x-o
            if(board[sl + 1][sc] == aim && board[sl - 2][sc] == aim)
            {
                boardKill[sl + 1][sc] = '+';
                boardKill[sl - 2][sc] = '+';
                num++;
            }
        }
    }

    //D TD BG
    if(sl != 1 && sl != 5 && sc != 1 && sc != 9)
    {
        if((oldPion_L < sl && oldPion_C < sc) || (oldPion_L > sl && oldPion_C > sc))
        {
            if(board[sl + 1][sc + 1] == aim && board[sl - 2][sc - 2] == aim)
            {

                boardKill[sl + 1][sc + 1] = '+';
                boardKill[sl - 2][sc - 2] = '+';
                num++;
            }

            if(board[sl - 1][sc - 1] == aim && board[sl + 2][sc + 2] == aim)
            {

                boardKill[sl - 1][sc - 1] = '+';
                boardKill[sl + 2][sc + 2] = '+';
                num++;
            }
        }

        if((oldPion_L < sl && oldPion_C > sc) || (oldPion_L > sl && oldPion_C < sc))
        {
            if(board[sl + 1][sc - 1] == aim && board[sl - 2][sc + 2] == aim)
            {

                boardKill[sl + 1][sc - 1] = '+';
                boardKill[sl - 2][sc + 2] = '+';
                num++;
            }
            if(board[sl - 1][sc + 1] == aim && board[sl + 2][sc - 2] == aim)
            {

                boardKill[sl - 1][sc + 1] = '+';
                boardKill[sl + 2][sc - 2] = '+';
                num++;
            }
        }


    }

    return num;
}

void set_move(int x, int y)
{

    if(x%2 != y%2)
    {
        for(int i = -1 ; i < 2; i++)
        {
            if(board[x-i][y] == 'w') board[x-i][y] = '+';
        }
        for(int j = -1 ; j < 2; j++)
        {
            if(board[x][y-j] == 'w') board[x][y-j] = '+';
        }
    }
    else
    {
        for(int i = -1 ; i < 2; i++)
        {
            for(int j = -1 ; j < 2; j++)
            {
                if(board[x-i][y-j] == 'w') board[x-i][y-j] = '+';

            }
        }

    }

}
void Move()
{
    board[selectedPion_L][selectedPion_C] = board[oldPion_L][oldPion_C];
    if(isCanReKill == false) playerOn = board[oldPion_L][oldPion_C] == 'X' ? 1 : 2;
    board[oldPion_L][oldPion_C] = 'w';
}

// mamadika ny vo select lasa kely
void InitBoard()
{
    for(int l = 1; l <= 5 ; l++)
    {
        for(int c = 1; c <= 9; c++)
        {
            if(board[l][c] == 'X') board[l][c] = 'x';
            if(board[l][c] == 'O') board[l][c] = 'o';
            if(board[l][c] == '+') board[l][c] = 'w';

        }
    }
}
void InitBoardKill()
{
    for(int l = 1; l <= 5 ; l++)
    {
        for(int c = 1; c <= 9; c++)
        {
            if(boardKill[l][c] == '+') boardKill[l][c] = 'w';


        }
    }
}


void ReFind()
{
    for(int l = 1; l <= 5 ; l++)
    {
        for(int c = 1; c <= 9; c++)
        {
            if(board[l][c] == 'x' && number_move(l,c) != 0 && playerOn == 2) board[l][c] = 'X';
            if(board[l][c] == 'o' && number_move(l,c) != 0 && playerOn == 1) board[l][c] = 'O';

            if(playerOn == 0)
            {
                if(board[l][c] == 'x' && number_move(l,c) != 0) board[l][c] = 'X';
                if(board[l][c] == 'o' && number_move(l,c) != 0) board[l][c] = 'O';
            }


        }
    }
}

int isCanKill(int a, int b, int _x,int _aim)
{
    int num = 0;
    char aim = _aim == 1 ? 'x' : 'o';

    {
        if(board[a][b + 1] == 'w')
        {

            if(board[a][b - 1] == aim || board[a][b + 2] == aim)
            {
                if(_x == 1) board[a][b + 1] = '+';
                num++;
            }
        }
        if(board[a][b - 1] == 'w')
        {

            if(board[a][b + 1] == aim || board[a][b - 2] == aim)
            {
                if(_x == 1) board[a][b - 1] = '+';
                num++;
            }
        }
        if(board[a + 1][b] == 'w')
        {
            if(board[a - 1][b] == aim || board[a + 2][b] == aim)
            {
                if(_x == 1) board[a + 1][b] = '+';
                num++;
            }
        }
        if(board[a - 1][b] == 'w')
        {
            if(board[a + 1][b] == aim || board[a - 2][b] == aim)
            {
                if(_x == 1) board[a - 1][b] = '+';
                num++;
            }
        }
        if(a%2 == b%2)
        {
            if(board[a + 1][b + 1] == 'w')
            {
                if(board[a - 1][b - 1] == aim || board[a + 2][b + 2] == aim)
                {
                    if(_x == 1) board[a + 1][b + 1] = '+';
                    num++;
                }
            }
            if(board[a - 1][b - 1] == 'w')
            {
                if(board[a + 1][b + 1] == aim || board[a - 2][b - 2] == aim)
                {
                    if(_x == 1) board[a - 1][b - 1] = '+';
                    num++;
                }
            }
            if(board[a + 1][b - 1] == 'w')
            {
                if(board[a - 1][b + 1] == aim || board[a + 2][b - 2] == aim)
                {
                    if(_x == 1) board[a + 1][b - 1] = '+';
                    num++;
                }
            }
            if(board[a - 1][b + 1] == 'w')
            {
                if(board[a + 1][b - 1] == aim || board[a - 2][b + 2] == aim)
                {
                    if(_x == 1) board[a - 1][b + 1] = '+';
                    num++;
                }
            }
        }

    }

    return num;
}
void CheckMustKill()
{
    char aim = playerOn == 1 ? 'o' : 'x';
    if(playerOn != 0)
    {
        for(int l = 1 ; l <= 5; l++)
        {
            for(int c = 1 ; c <= 9; c++)
            {

                if(board[l][c] == 'O' && aim == 'o')
                {
                    if(isCanKill(l,c,0,playerOn) == 0) board[l][c] = aim;
                    else numCanEat++;

                }
                else if(board[l][c] == 'X' && aim == 'x')
                {
                    if(isCanKill(l,c,0,playerOn) == 0) board[l][c] = aim;
                    else numCanEat++;
                }

            }
        }
    }


}
void Killing(int ol, int oc, int sl, int sc)
{
    char aim = playerOn == 1 ? 'o' : 'x';
    char aimOp = aim == 'o' ? 'x' : 'o';
    //H kill
    if(ol == sl)
    {

        if(oc > sc)
        {
            for(int i = sc - 1; i >= 1; i--)
            {
                if(board[ol][i] == aimOp || board[ol][i] == 'w' || board[ol][i] == '.') break;
                if(board[ol][i] == aim)
                {
                    killed++;
                    board[ol][i] = 'w';
                }
            }
        }
        else if(oc < sc)
        {

            for(int i = sc + 1; i <= 9; i++)
            {

                if(board[sl][i] == aimOp || board[sl][i] == 'w' || board[ol][i] == '.') break;
                if(board[sl][i] == aim)
                {
                    killed++;
                    board[sl][i] = 'w';
                }

            }
        }
    }

    // V kill
    if(oc == sc)
    {

        if(ol > sl)
        {
            for(int i = sl - 1; i >= 1; i--)
            {
                if(board[i][oc] == aimOp || board[i][oc] == 'w' || board[i][oc] == '.') break;
                if(board[i][oc] == aim)
                {
                    killed++;
                    board[i][oc] = 'w';
                }
            }
        }
        else if(ol < sl)
        {

            for(int i = sl + 1; i <= 5; i++)
            {

                if(board[i][oc] == aimOp || board[i][oc] == 'w' || board[i][oc] == '.') break;
                if(board[i][oc] == aim)
                {
                    killed++;
                    board[i][oc] = 'w';
                }

            }
        }
    }
    // D kill
    if(ol < sl && oc < sc)
    {
        int pc = sc + 1;
        for(int i = sl + 1; i <= 5; i++)
        {
            if(board[i][pc] == aimOp || board[i][pc] == 'w' || board[i][pc] == '.') break;
            if(board[i][pc] == aim)
            {
                killed++;
                board[i][pc] = 'w';
            }
            pc++;
        }
    }
    if(ol > sl && oc > sc)
    {
        int pc = sc - 1;
        for(int i = sl - 1; i >= 1; i--)
        {
            if(board[i][pc] == aimOp || board[i][pc] == 'w' || board[i][pc] == '.') break;
            if(board[i][pc] == aim)
            {
                killed++;
                board[i][pc] = 'w';
            }
            pc--;
        }
    }
    if(ol > sl && oc < sc)
    {
        int pc = sc + 1;
        for(int i = sl - 1; i >= 1; i--)
        {
            if(board[i][pc] == aimOp || board[i][pc] == 'w' || board[i][pc] == '.') break;
            if(board[i][pc] == aim)
            {
                killed++;
                board[i][pc] = 'w';
            }
            pc++;
        }
    }
    if(ol < sl && oc > sc)
    {
        int pc = sc - 1;
        for(int i = sl + 1; i <= 5; i++)
        {
            if(board[i][pc] == aimOp || board[i][pc] == 'w' || board[i][pc] == '.') break;
            if(board[i][pc] == aim)
            {
                killed++;
                board[i][pc] = 'w';
            }
            pc--;

        }
    }
}

void Kill(int ol, int oc, int sl, int sc)
{

    //H
    char aim = playerOn == 1 ? 'o' : 'x';
    if(ol == sl)
    {
        if(oc > sc)
        {
            if(board[ol][oc + 1] == aim) Killing(ol,oc - 1,ol,oc);
            else Killing(ol,oc,sl,sc);
        }

        if(oc < sc)
        {
            if(board[ol][oc - 1] == aim) Killing(ol,oc + 1,ol,oc);
            else Killing(ol,oc,sl,sc);
        }
    }

    //V
    if(oc == sc)
    {
        if(ol > sl)
        {
            if(board[ol + 1][oc] == aim) Killing(ol - 1,oc,ol,oc);
            else Killing(ol,oc,sl,sc);
        }

        if(ol < sl)
        {
            if(board[ol - 1][oc] == aim) Killing(ol + 1,oc,ol,oc);
            else Killing(ol,oc,sl,sc);
        }
    }

    //D
    if(ol < sl && oc < sc)
    {
        if(board[ol - 1][oc - 1] == aim) Killing(ol + 1,oc + 1,ol,oc);
        else Killing(ol,oc,sl,sc);
    }
    if(ol > sl && oc > sc)
    {
        if(board[ol + 1][oc + 1] == aim) Killing(ol - 1,oc - 1,ol,oc);
        else Killing(ol,oc,sl,sc);
    }
    if(ol > sl && oc < sc)
    {
        if(board[ol + 1][oc - 1] == aim) Killing(ol - 1,oc + 1,ol,oc);
        else Killing(ol,oc,sl,sc);
    }
    if(ol < sl && oc > sc)
    {
        if(board[ol - 1][oc + 1] == aim) Killing(ol + 1,oc - 1,ol,oc);
        else Killing(ol,oc,sl,sc);
    }

}

void KillChoose(int ol, int oc, int sl, int sc)
{
    //H
    if(ol == sl)
    {
        if(oc > sc)Killing(ol,oc + 1,ol,sc + 1);
        if(oc < sc)Killing(ol,oc - 1,ol,sc - 1);
    }

    if(oc == sc)
    {
        if(ol > sl)Killing(ol + 1,oc,sl + 1,oc);
        if(ol < sl)Killing(ol - 1,oc,sl - 1,oc);
    }

    //D
    if(ol > sl && oc > sc) Killing(ol + 1,oc + 1,sl + 1,sc + 1);
    if(ol < sl && oc < sc) Killing(ol - 1,oc - 1,sl - 1,sc - 1);
    if(ol > sl && oc < sc) Killing(ol + 1,oc - 1,sl + 1,sc - 1);
    if(ol < sl && oc > sc) Killing(ol - 1,oc + 1,sl - 1,sc + 1);

}

void InitBoardMove()
{
    for(int i = 1; i <= 5; i++)
    {
        for(int j = 1; j <= 9; j++)
        {
            boardMove[i][j] = 'w';
        }
    }
}

HINSTANCE instance;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    instance = hThisInstance;
    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Fanorona  MISEI 2023"),       /* Title Text */
               WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               900,                 /* The programs width */
               550,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    //jeu


    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{



    int testId;
    static HWND button = NULL;
    switch (message)                  /* handle the messages */
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // Dessin d'une ligne
        HPEN hPen = CreatePen(PS_SOLID,3, RGB(128, 128, 128));
        HGDIOBJ oldPen = SelectObject(hdc, hPen);
        MoveToEx(hdc, initX, initY, NULL);
        LineTo(hdc, (siseCase * 8) + initX, initY);
        LineTo(hdc, (siseCase * 8) + initX, (siseCase * 4) + initY);
        LineTo(hdc, initX, (siseCase * 4) + initY);
        LineTo(hdc, initX, initY);

        for(int i = 1; i <= 8 ; i++)
        {
            MoveToEx(hdc, (siseCase * i) + initX, initY, NULL);
            LineTo(hdc, (siseCase * i)+ initX, (siseCase * 4) + initY);
        }
        for(int i = 1; i <= 3 ; i++)
        {
            MoveToEx(hdc, initX, (siseCase * i) + initY, NULL);
            LineTo(hdc,(siseCase * 8) + initX, (siseCase * i)+ initY);
        }
        for(int i = 0; i <= 4 ; i+=2)
        {
            MoveToEx(hdc, (siseCase * i) + initX, initY, NULL);
            LineTo(hdc, (siseCase * 4) + (siseCase * i) + initX, (siseCase * 4) + initY);

            MoveToEx(hdc, (siseCase * 8) - (siseCase * i) + initX, initY, NULL);
            LineTo(hdc, (siseCase * 8) - (siseCase * i) - (siseCase * 4) + initX, (siseCase * 4)+initY);
        }

        MoveToEx(hdc, (siseCase * 2) + initX, initY, NULL);
        LineTo(hdc, initX, (siseCase * 2)+ initY);
        LineTo(hdc, (siseCase * 2) + initX, (siseCase * 4)+initY);

        MoveToEx(hdc, (siseCase * 8) - (siseCase * 2) + initX, initY, NULL);
        LineTo(hdc, (siseCase * 8) + initX, (siseCase * 2)+initY);
        LineTo(hdc, (siseCase * 8) - (siseCase * 2) + initX, (siseCase * 4)+initY);

        SelectObject(hdc, oldPen);
        DeleteObject(hPen);
        // Dessin  cercle

        HBRUSH hBrush;
        HGDIOBJ oldBrush;

        for(int j = 1 ; j <= 5; j++)
        {
            for(int i = 1; i <= 9; i++)
            {
                if(boardKill[j][i] == '+')
                {
                    hBrush = CreateSolidBrush(RGB(0, 200, 0));
                    sizePion = 20;
                    oldBrush = SelectObject(hdc, hBrush);
                    Ellipse(hdc, initX + (siseCase * (i-1)) - sizePion, initY + (siseCase * (j-1)) - sizePion, initX + (siseCase * (i-1)) + sizePion, initY + (siseCase * (j-1)) + sizePion);
                }
            }
        }

        for(int j = 1 ; j <= 5; j++)
        {
            for(int i = 1; i <= 9; i++)
            {
                if(i == selectedPion_C && j == selectedPion_L)
                {

                    switch (board[j][i])
                    {

                    case 'X':
                        hBrush = CreateSolidBrush(RGB(50, 150, 255));
                        sizePion = 15;
                        break;
                    case 'O':
                        hBrush = CreateSolidBrush(RGB(255, 150, 50));
                        sizePion = 15;
                        break;
                    case 'x':
                        hBrush = CreateSolidBrush(RGB(0, 100, 255));
                        sizePion = 10;
                        break;
                    case 'o':
                        hBrush = CreateSolidBrush(RGB(255, 50, 0));
                        sizePion = 10;
                        break;
                    default:

                        hBrush = CreateSolidBrush(RGB(128, 128, 128));
                        sizePion = 2;

                        break;
                    }

                }

                else
                {
                    switch (board[j][i])
                    {
                    case 'x':
                        hBrush = CreateSolidBrush(RGB(0, 100, 255));
                        sizePion = 10;
                        break;
                    case 'o':
                        hBrush = CreateSolidBrush(RGB(255, 50, 0));
                        sizePion = 10;
                        break;
                    case 'X':
                        hBrush = CreateSolidBrush(RGB(0, 100, 255));
                        sizePion = 15;
                        break;
                    case 'O':
                        hBrush = CreateSolidBrush(RGB(255, 50, 0));
                        sizePion = 15;
                        break;
                    case '+':
                        hBrush = CreateSolidBrush(RGB(128,128, 128));
                        sizePion = 10;
                        break;
                    case 'w':
                        hBrush = CreateSolidBrush(RGB(128,128, 128));
                        sizePion = 2;
                        break;
                    default:
                        hBrush = CreateSolidBrush(RGB(128, 128, 128));
                        sizePion = 2;
                        break;
                    }
                }

                oldBrush = SelectObject(hdc, hBrush);
                Ellipse(hdc, initX + (siseCase * (i-1)) - sizePion, initY + (siseCase * (j-1)) - sizePion, initX + (siseCase * (i-1)) + sizePion, initY + (siseCase * (j-1)) + sizePion);
            }
        }

        SelectObject(hdc, oldBrush);

        SelectObject(hdc,  CreateFont(
                         -MulDiv(20, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                         0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0, "Tahoma"));

        SetBkMode (hdc, TRANSPARENT) ;


        TextOut (hdc, initX + 250, 470,  phrase[msgScreen],  strlen(phrase[msgScreen]));
        SelectObject(hdc,  CreateFont(
                         -MulDiv(14, GetDeviceCaps(hdc, LOGPIXELSY), 72),
                         0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0, "Tahoma"));
        char phrase[14][2] = {"1","2","3","4","5","A","B","C","D","E","F","G","H","I"};

        for(int i = 0 ; i < 5 ; i++)
        {
            TextOut (hdc, initX - 35, initY + (siseCase * i) - 10, phrase[i], strlen(phrase[i]));
        }

        for(int j = 5 ; j < 14 ; j++)
        {
            TextOut (hdc, initX + (siseCase * (j-5)) - 5, initY - 40, phrase[j], strlen(phrase[j]));
        }
        DeleteObject(hBrush);


        EndPaint(hwnd, &ps);


        return 0;

    }
    case WM_CREATE:

        button = CreateWindow("BUTTON",
                                  "Reinitialiser",
                                  WS_CHILD | WS_VISIBLE,
                                  50,470,100,30,
                                  hwnd,
                                  (HMENU)0,instance,NULL);

        return 0;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case 0:
            testId = MessageBox(hwnd, "Reinitialiser le plato ?", "test", MB_YESNO);
            if(testId == IDYES)
            {
                for(int l = 1; l <= 5 ; l++)
                {
                    for(int c = 1; c <= 9; c++)
                    {
                        board[l][c] = boardInit[l][c];
                    }
                }
                oldPion_L  = oldPion_C = selectedPion_L = selectedPion_C = 0;
                mustChoose = false;
                isCanReKill = false;
                playerOn = 0;
                oldPosChooseC = oldPosChooseL = 0;
                InitBoardMove();
                InitBoardKill();
                msgScreen = 0;
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
        return 0;
    case WM_LBUTTONDOWN:
    {
        int selectedPionX  = 0;
        int selectedPionY  = 0;
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        for(int i = 1; i <= 5; i++)
        {
            if(yPos >= initY + (siseCase * (i-1)) - 10 && yPos <= initY + (siseCase * (i-1)) + 10) selectedPionX = i;
        }
        for(int j = 1; j < 10; j++)
        {
            if(xPos >= initX + (siseCase * (j-1)) - 10 && xPos <= initX + (siseCase * (j-1)) + 10) selectedPionY = j;
        }
        selectedPionY = selectedPionY > 9 ? 0 : selectedPionY;
        selectedPion_L = selectedPionX >= 1 && selectedPionY != 0 ? selectedPionX : 0;
        selectedPion_C = selectedPionY >= 1 && selectedPionX != 0? selectedPionY : 0;


        if(oldPion_C != 0 && selectedPion_C != 0 && board[selectedPion_L][selectedPion_C] == '+' && mustChoose == false)
        {
            killed = 0;
            InitBoardKill();
            Move();
            InitBoard();

            oldPosChooseL = oldPion_L;
            oldPosChooseC = oldPion_C;

            //verificationsi le pion peut faire un choix
            if(isCanChoose(selectedPion_L, selectedPion_C) != 0)
            {
                boardMove[oldPosChooseL][oldPosChooseC] = '+';
                oldPion_L = selectedPion_L;
                oldPion_C = selectedPion_C;
                mustChoose = true;
            }
            else
            {
                int _aim = playerOn == 1 ? 2 : 1;
                Kill(oldPion_L,oldPion_C,selectedPion_L, selectedPion_C);

                // Check ReKill
                //
                if(isCanKill(selectedPion_L,selectedPion_C,0,_aim) != 0 && killed != 0)
                {
                    printf("can re kill\n");
                    isCanReKill = true;
                    boardMove[oldPosChooseL][oldPosChooseC] = '+';
                }
                else
                {
                    isCanReKill = false;
                    InitBoardMove();
                    oldPosChooseL = oldPosChooseC = 0;
                }

                if(isCanReKill == false)
                {
                    ReFind();

                    //izay afaka mihinana ihany no mandeha
                    //
                    CheckMustKill();
                    if(numCanEat == 0) ReFind();
                    numCanEat = 0;
                    //
                }
                else
                {
                    int _aim = playerOn == 1 ? 2 : 1;
                    isCanKill(selectedPion_L,selectedPion_C,1,_aim);
                    oldPion_L = selectedPion_L;
                    oldPion_C = selectedPion_C;

                    for(int i = 1; i <= 5; i++)
                    {
                        for(int j = 1; j <= 9; j++)
                        {
                            if(boardMove[i][j] == '+') board[i][j] = 'w';
                        }
                    }

                    int posDispo = 0;

                    for(int i = 1; i <= 5; i++)
                    {
                        for(int j = 1; j <= 9; j++)
                        {
                            if(board[i][j] == '+') posDispo++;
                        }
                    }

                    if(posDispo == 0)
                    {
                        isCanReKill = false;
                        mustChoose = false;
                        InitBoardMove();
                        ReFind();

                        //izay afaka mihinana ihany no mandeha
                        //
                        CheckMustKill();
                        if(numCanEat == 0) ReFind();
                        numCanEat = 0;
                    }

                }

            }

            if(playerOn == 1 && isCanReKill == false && mustChoose == false) msgScreen = 1;
            if(playerOn == 2 && isCanReKill == false && mustChoose == false) msgScreen = 2;

            int canMoveX = 0;
            int canMoveO = 0;

            for(int i = 1; i <= 5; i++)
            {
                for(int j = 1; j <= 9; j++)
                {
                    if(board[i][j] == 'X') canMoveX++;
                    if(board[i][j] == 'O') canMoveO++;

                }
            }

            if(playerOn == 2 && canMoveX == 0 && isCanReKill == false && mustChoose == false) msgScreen = 3;
            if(playerOn == 1 && canMoveO == 0 && isCanReKill == false && mustChoose == false) msgScreen = 4;

            InvalidateRect(hwnd, NULL, TRUE);
        }

        if(mustChoose == true)
        {

            if(boardKill[selectedPion_L][selectedPion_C] == '+')
            {

                KillChoose(oldPion_L,oldPion_C,selectedPion_L, selectedPion_C);
                InitBoardKill();
                mustChoose = false;

                // Check ReKill
                //
                int _aim = playerOn == 1 ? 2 : 1;
                if(isCanKill(oldPion_L,oldPion_C,0,_aim) != 0)
                {
                    isCanReKill = true;
                    //boardMove[oldPosChooseL][oldPosChooseC] = '+';
                }
                else
                {
                    isCanReKill = false;
                    InitBoardMove();
                    oldPosChooseL = oldPosChooseC = 0;
                }

                //

                if(isCanReKill == false)
                {
                    ReFind();

                    //izay afaka mihinana ihany no mandeha
                    //
                    CheckMustKill();
                    if(numCanEat == 0) ReFind();
                    numCanEat = 0;
                    //
                }
                else
                {
                    int _aim = playerOn == 1 ? 2 : 1;
                    isCanKill(oldPion_L,oldPion_C,1,_aim);

                    for(int i = 1; i <= 5; i++)
                    {
                        for(int j = 1; j <= 9; j++)
                        {
                            if(boardMove[i][j] == '+') board[i][j] = 'w';
                        }
                    }

                    int posDispo = 0;

                    for(int i = 1; i <= 5; i++)
                    {
                        for(int j = 1; j <= 9; j++)
                        {
                            if(board[i][j] == '+') posDispo++;
                        }
                    }

                    if(posDispo == 0)
                    {
                        isCanReKill = false;
                        mustChoose = false;
                        InitBoardMove();
                        ReFind();

                        //izay afaka mihinana ihany no mandeha
                        //
                        CheckMustKill();
                        if(numCanEat == 0) ReFind();
                        numCanEat = 0;

                    }


                }

            }

            if(playerOn == 1 && isCanReKill == false && mustChoose == false) msgScreen = 1;
            if(playerOn == 2 && isCanReKill == false && mustChoose == false) msgScreen = 2;

            InvalidateRect(hwnd, NULL, TRUE);
        }



        if(selectedPion_C != 0 && board[selectedPion_L][selectedPion_C] != 'w' && board[selectedPion_L][selectedPion_C] != '+' && mustChoose == false && isCanReKill == false)
        {

            for(int l = 1; l <= 5 ; l++)
            {
                for(int c = 1; c <= 9; c++)
                {
                    if(board[l][c] == '+') board[l][c] = 'w';
                }
            }
            if(board[selectedPion_L][selectedPion_C] == 'X' || board[selectedPion_L][selectedPion_C] == 'O')
            {
                if(isCanKill(selectedPion_L,selectedPion_C,1,playerOn) == 0)
                {
                    set_move(selectedPion_L, selectedPion_C);
                }

            }
            oldPion_L  = selectedPion_L;
            oldPion_C  = selectedPion_C;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else if((selectedPion_C == 0 || board[selectedPion_L][selectedPion_C] == 'w') && mustChoose == false && isCanReKill == false)
        {
            oldPion_L  = oldPion_C = 0;
            InitBoard();
            ReFind();

            //izay afaka mihinana ihany no mandeha
            CheckMustKill();
            if(numCanEat == 0) ReFind();
            numCanEat = 0;


            InvalidateRect(hwnd, NULL, TRUE);

        }


        return 0;
    }

    case WM_DESTROY:

        PostQuitMessage (0);
        break;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}




