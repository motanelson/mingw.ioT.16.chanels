#include <windows.h>
#include <stdio.h>
//usr/bin/i686-w64-mingw32-gcc -o grid_win grid_win.c -lgdi32 -mwindows

#define WIN_W 800
#define WIN_H 600
#define CELLS_PER_ROW 4
#define CELLS_PER_COL 4
#define CELL_SIZE 50
#define GAP 10

int states[CELLS_PER_ROW * CELLS_PER_COL] = {0};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // fundo amarelo
        HBRUSH hBrushYellow = CreateSolidBrush(RGB(255, 255, 0));
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, hBrushYellow);
        DeleteObject(hBrushYellow);

        // calcular offsets para centrar
        int grid_w = CELLS_PER_ROW * CELL_SIZE + (CELLS_PER_ROW - 1) * GAP;
        int grid_h = CELLS_PER_COL * CELL_SIZE + (CELLS_PER_COL - 1) * GAP;
        int start_x = (WIN_W - grid_w) / 2;
        int start_y = (WIN_H - grid_h) / 2;

        for (int r = 0; r < CELLS_PER_COL; r++) {
            for (int c = 0; c < CELLS_PER_ROW; c++) {
                int idx = r * CELLS_PER_ROW + c;
                int x = start_x + c * (CELL_SIZE + GAP);
                int y = start_y + r * (CELL_SIZE + GAP);

                HBRUSH hBrush = CreateSolidBrush(states[idx] ? RGB(0,0,0) : RGB(255,255,255));
                RECT cell = {x, y, x + CELL_SIZE, y + CELL_SIZE};
                FillRect(hdc, &cell, hBrush);
                DeleteObject(hBrush);

                // borda preta
                FrameRect(hdc, &cell, GetStockObject(BLACK_BRUSH));
            }
        }

        EndPaint(hwnd, &ps);
    } break;

    case WM_LBUTTONDOWN: {
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);

        int grid_w = CELLS_PER_ROW * CELL_SIZE + (CELLS_PER_ROW - 1) * GAP;
        int grid_h = CELLS_PER_COL * CELL_SIZE + (CELLS_PER_COL - 1) * GAP;
        int start_x = (WIN_W - grid_w) / 2;
        int start_y = (WIN_H - grid_h) / 2;

        if (mx >= start_x && mx < start_x + grid_w &&
            my >= start_y && my < start_y + grid_h) {
            int relx = mx - start_x;
            int rely = my - start_y;
            int col = relx / (CELL_SIZE + GAP);
            int row = rely / (CELL_SIZE + GAP);

            if (col >= 0 && col < CELLS_PER_ROW && row >= 0 && row < CELLS_PER_COL) {
                int cell_x = col * (CELL_SIZE + GAP);
                int cell_y = row * (CELL_SIZE + GAP);
                int inside_x = relx - cell_x;
                int inside_y = rely - cell_y;
                if (inside_x < CELL_SIZE && inside_y < CELL_SIZE) {
                    int idx = row * CELLS_PER_ROW + col;
                    states[idx] = states[idx] ? 0 : 1;

                    // redesenhar
                    InvalidateRect(hwnd, NULL, TRUE);

                    // chamar comando
                    char cmd[128];
                    sprintf(cmd, "command%d %d", idx + 1, states[idx]);
                    system(cmd);
                }
            }
        }
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GridWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("GridWindow", "Janela Amarela",
                             WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             WIN_W, WIN_H,
                             NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
