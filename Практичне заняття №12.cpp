#include <windows.h>
#include <cmath>
#include <algorithm>

using namespace std;

const int TOP_MARGIN = 80;
const double PI_VAL = 3.14159265358979323846;

// ---------------- Структура для "людини" (анімація) ----------------

struct StickmanState {
    int num;
    int H;
    int Xpos;
    int Ypos;
    int Hmen;
    int Rhead;
    int Rhead2;
    int revers;
    int L;
};

StickmanState gStick;
bool gAnimRunning = false;

// ---------------- Допоміжні процедури малювання ----------------

void ClearDrawing(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    rc.top = TOP_MARGIN;
    HDC hdc = GetDC(hwnd);
    HBRUSH hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
    FillRect(hdc, &rc, hbr);
    ReleaseDC(hwnd, hdc);
}

void DrawAstroidPixels(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - TOP_MARGIN;
    int cx = width / 2;
    int cy = TOP_MARGIN + height / 2;
    int a = min(width, height) / 4;

    HDC hdc = GetDC(hwnd);
    for (int i = 0; i <= 360; ++i) {
        double t = 2.0 * PI_VAL * i / 360.0;
        double x = a * pow(cos(t), 3);
        double y = a * pow(sin(t), 3);
        int px = cx + (int)x;
        int py = cy - (int)y;
        SetPixel(hdc, px, py, RGB(0, 0, 255));
    }
    ReleaseDC(hwnd, hdc);
}

void DrawAstroidPen(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - TOP_MARGIN;
    int cx = width / 2;
    int cy = TOP_MARGIN + height / 2;
    int a = min(width, height) / 4;

    HDC hdc = GetDC(hwnd);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, hPen);

    bool first = true;
    for (int i = 0; i <= 360; ++i) {
        double t = 2.0 * PI_VAL * i / 360.0;
        double x = a * pow(cos(t), 3);
        double y = a * pow(sin(t), 3);
        int px = cx + (int)x;
        int py = cy - (int)y;
        if (first) {
            MoveToEx(hdc, px, py, nullptr);
            first = false;
        }
        else {
            LineTo(hdc, px, py);
        }
    }

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
    ReleaseDC(hwnd, hdc);
}

void DrawShapes(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int left = 20;
    int top = TOP_MARGIN + 10;

    HDC hdc = GetDC(hwnd);
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    Arc(hdc, left, top, left + 80, top + 80, left + 80, top + 40, left, top + 40);
    TextOutA(hdc, left + 25, top + 85, "Arc", 3);

    int x = left + 100;
    Chord(hdc, x, top, x + 80, top + 80, x + 80, top + 40, x, top + 40);
    TextOutA(hdc, x + 20, top + 85, "Chord", 5);

    x = left + 200;
    Ellipse(hdc, x, top, x + 80, top + 40);
    TextOutA(hdc, x + 20, top + 45, "Ellipse", 7);

    x = left + 300;
    Pie(hdc, x, top, x + 80, top + 80, x + 80, top + 20, x, top + 20);
    TextOutA(hdc, x + 25, top + 85, "Pie", 3);

    POINT pts[5];
    pts[0].x = left + 10;
    pts[0].y = top + 120;
    pts[1].x = left + 20;
    pts[1].y = top + 100;
    pts[2].x = left + 30;
    pts[2].y = top + 110;
    pts[3].x = left + 40;
    pts[3].y = top + 100;
    pts[4].x = left + 50;
    pts[4].y = top + 120;
    Polygon(hdc, pts, 5);
    TextOutA(hdc, left + 10, top + 125, "Polygon", 7);

    for (int i = 0; i < 5; ++i) pts[i].x += 120;
    Polyline(hdc, pts, 5);
    TextOutA(hdc, left + 130, top + 125, "Polyline", 8);

    Rectangle(hdc, left + 250, top + 110, left + 300, top + 150);
    TextOutA(hdc, left + 250, top + 155, "Rectangle", 9);

    RoundRect(hdc, left + 360, top + 110, left + 410, top + 150, 20, 20);
    TextOutA(hdc, left + 355, top + 155, "RoundRect", 9);

    SelectObject(hdc, oldFont);
    ReleaseDC(hwnd, hdc);
}

void DrawLineStyles(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;

    HDC hdc = GetDC(hwnd);
    int styles[7] = {
        PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT,
        PS_DASHDOTDOT, PS_NULL, PS_INSIDEFRAME
    };

    for (int i = 0; i < 7; ++i) {
        HPEN pen = CreatePen(styles[i], 1, RGB(0, 0, 0));
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        int y = TOP_MARGIN + 20 + i * 20;
        MoveToEx(hdc, 20, y, nullptr);
        LineTo(hdc, width - 20, y);
        SelectObject(hdc, oldPen);
        DeleteObject(pen);
    }
    ReleaseDC(hwnd, hdc);
}

void DrawSine(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - TOP_MARGIN;
    int midY = TOP_MARGIN + height / 2;
    int halfW = width / 2;

    HDC hdc = GetDC(hwnd);

    MoveToEx(hdc, 0, midY, nullptr);
    LineTo(hdc, width, midY);

    for (int px = 0; px < halfW; ++px) {
        double X = px * 4.0 * PI_VAL / halfW;
        double Y = sin(X);
        int py = midY - (int)(Y * (height / 2 - 10));
        SetPixel(hdc, px, py, RGB(0, 0, 0));
    }

    int startX = halfW;
    bool first = true;
    for (int px = 0; px < halfW; ++px) {
        double X = px * 4.0 * PI_VAL / halfW;
        double Y = sin(X);
        int py = midY - (int)(Y * (height / 2 - 10));
        if (first) {
            MoveToEx(hdc, startX + px, py, nullptr);
            first = false;
        }
        else {
            LineTo(hdc, startX + px, py);
        }
    }

    ReleaseDC(hwnd, hdc);
}

// ---------------- Анімація: ініціалізація та один кадр ----------------

void InitStickman(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - TOP_MARGIN;

    gStick.num = 0;
    gStick.H = 30;
    gStick.Xpos = 2 * gStick.H;
    gStick.Ypos = TOP_MARGIN + height - 40;
    gStick.Hmen = 30;
    gStick.Rhead = 10;
    gStick.Rhead2 = gStick.Rhead / 2;
    gStick.revers = 1;
    gStick.L = (int)(gStick.H * 1.41);
}

void DrawStickmanFrame(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    HDC hdc = GetDC(hwnd);

    RECT rfill = rc;
    rfill.top = TOP_MARGIN;
    HBRUSH hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
    FillRect(hdc, &rfill, hbr);

    HPEN penGround = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
    HPEN oldPen = (HPEN)SelectObject(hdc, penGround);
    MoveToEx(hdc, 0, gStick.Ypos + 3, nullptr);
    LineTo(hdc, rc.right, gStick.Ypos + 3);
    SelectObject(hdc, oldPen);
    DeleteObject(penGround);

    int Yhead;
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    oldPen = (HPEN)SelectObject(hdc, pen);

    if (gStick.num == 0) {
        Yhead = gStick.Ypos - gStick.H - gStick.Hmen;

        MoveToEx(hdc, gStick.Xpos - gStick.H, gStick.Ypos, nullptr);
        LineTo(hdc, gStick.Xpos, gStick.Ypos - gStick.H);
        LineTo(hdc, gStick.Xpos + gStick.H, gStick.Ypos);

        MoveToEx(hdc, gStick.Xpos, gStick.Ypos - gStick.H, nullptr);
        LineTo(hdc, gStick.Xpos, Yhead);

        MoveToEx(hdc, gStick.Xpos + gStick.revers * gStick.H, Yhead - gStick.H, nullptr);
        LineTo(hdc, gStick.Xpos, Yhead + 4);
        Ellipse(hdc,
            gStick.Xpos + gStick.revers * gStick.H - gStick.Rhead2,
            Yhead - gStick.H - gStick.Rhead2,
            gStick.Xpos + gStick.revers * gStick.H + gStick.Rhead2,
            Yhead - gStick.H + gStick.Rhead2);

        LineTo(hdc, gStick.Xpos + gStick.revers * gStick.H, Yhead + gStick.H);
        Ellipse(hdc,
            gStick.Xpos + gStick.revers * gStick.H - gStick.Rhead2,
            Yhead + gStick.H - gStick.Rhead2,
            gStick.Xpos + gStick.revers * gStick.H + gStick.Rhead2,
            Yhead + gStick.H + gStick.Rhead2);

        Ellipse(hdc,
            gStick.Xpos - gStick.Rhead,
            Yhead - 2 * gStick.Rhead,
            gStick.Xpos + gStick.Rhead,
            Yhead);
    }
    else {
        Yhead = gStick.Ypos - gStick.L - gStick.Hmen;

        MoveToEx(hdc, gStick.Xpos, gStick.Ypos, nullptr);
        LineTo(hdc, gStick.Xpos, Yhead);

        MoveToEx(hdc, gStick.Xpos, Yhead + 4, nullptr);
        LineTo(hdc, gStick.Xpos + gStick.revers * gStick.L, Yhead + 4);
        Ellipse(hdc,
            gStick.Xpos + gStick.revers * gStick.L - gStick.Rhead2,
            Yhead + 4 - gStick.Rhead2,
            gStick.Xpos + gStick.revers * gStick.L + gStick.Rhead2,
            Yhead + 4 + gStick.Rhead2);

        Ellipse(hdc,
            gStick.Xpos - gStick.Rhead,
            Yhead - 2 * gStick.Rhead,
            gStick.Xpos + gStick.Rhead,
            Yhead);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
    ReleaseDC(hwnd, hdc);

    if (gStick.Xpos >= rc.right - gStick.H || gStick.Xpos <= gStick.H)
        gStick.revers = -gStick.revers;
    gStick.Xpos += gStick.revers * gStick.H;
    gStick.num = 1 - gStick.num;
}

// ---------------- Глобальні дескриптори кнопок ----------------

HINSTANCE g_hInst;
HWND hBtnAstroidPix;
HWND hBtnAstroidPen;
HWND hBtnShapes;
HWND hBtnLineStyles;
HWND hBtnSine;
HWND hBtnAnim;
HWND hBtnClear;

#define ID_BTN_ASTROID_PIX   101
#define ID_BTN_ASTROID_PEN   102
#define ID_BTN_SHAPES        103
#define ID_BTN_LINESTYLES    104
#define ID_BTN_SINE          105
#define ID_BTN_ANIM          106
#define ID_BTN_CLEAR         107
#define ID_TIMER_ANIM        201

// ---------------- WndProc ----------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
    {
        CreateWindowExA(0, "BUTTON", "Управлiння",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 10, 780, 60,
            hwnd, nullptr, g_hInst, nullptr);

        hBtnAstroidPix = CreateWindowExA(0, "BUTTON", "Астроїда (пiкселi)",
            WS_CHILD | WS_VISIBLE,
            20, 30, 130, 25,
            hwnd, (HMENU)ID_BTN_ASTROID_PIX, g_hInst, nullptr);

        hBtnAstroidPen = CreateWindowExA(0, "BUTTON", "Астроїда (перо)",
            WS_CHILD | WS_VISIBLE,
            160, 30, 130, 25,
            hwnd, (HMENU)ID_BTN_ASTROID_PEN, g_hInst, nullptr);

        hBtnShapes = CreateWindowExA(0, "BUTTON", "Фiгури",
            WS_CHILD | WS_VISIBLE,
            300, 30, 80, 25,
            hwnd, (HMENU)ID_BTN_SHAPES, g_hInst, nullptr);

        hBtnLineStyles = CreateWindowExA(0, "BUTTON", "Стилi лiнiй",
            WS_CHILD | WS_VISIBLE,
            390, 30, 100, 25,
            hwnd, (HMENU)ID_BTN_LINESTYLES, g_hInst, nullptr);

        hBtnSine = CreateWindowExA(0, "BUTTON", "Синусоїда",
            WS_CHILD | WS_VISIBLE,
            500, 30, 100, 25,
            hwnd, (HMENU)ID_BTN_SINE, g_hInst, nullptr);

        hBtnAnim = CreateWindowExA(0, "BUTTON", "Пуск анiмацiї",
            WS_CHILD | WS_VISIBLE,
            610, 30, 120, 25,
            hwnd, (HMENU)ID_BTN_ANIM, g_hInst, nullptr);

        hBtnClear = CreateWindowExA(0, "BUTTON", "Очистити",
            WS_CHILD | WS_VISIBLE,
            740, 30, 80, 25,
            hwnd, (HMENU)ID_BTN_CLEAR, g_hInst, nullptr);

        SetTimer(hwnd, ID_TIMER_ANIM, 500, nullptr);
        gAnimRunning = false;
        ClearDrawing(hwnd);
    }
    break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_BTN_ASTROID_PIX:
            ClearDrawing(hwnd);
            DrawAstroidPixels(hwnd);
            break;
        case ID_BTN_ASTROID_PEN:
            ClearDrawing(hwnd);
            DrawAstroidPen(hwnd);
            break;
        case ID_BTN_SHAPES:
            ClearDrawing(hwnd);
            DrawShapes(hwnd);
            break;
        case ID_BTN_LINESTYLES:
            ClearDrawing(hwnd);
            DrawLineStyles(hwnd);
            break;
        case ID_BTN_SINE:
            ClearDrawing(hwnd);
            DrawSine(hwnd);
            break;
        case ID_BTN_ANIM:
            gAnimRunning = !gAnimRunning;
            if (gAnimRunning) {
                InitStickman(hwnd);
                SetWindowTextA(hBtnAnim, "Стоп анiмацiї");
            }
            else {
                SetWindowTextA(hBtnAnim, "Пуск анiмацiї");
            }
            break;
        case ID_BTN_CLEAR:
            ClearDrawing(hwnd);
            break;
        }
        break;

    case WM_TIMER:
        if (wParam == ID_TIMER_ANIM && gAnimRunning) {
            DrawStickmanFrame(hwnd);
        }
        break;

    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER_ANIM);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ---------------- main: Console-проєкт з GUI ----------------

int main() {
    FreeConsole();

    g_hInst = GetModuleHandle(nullptr);

    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "GraphicsWndClass";

    if (!RegisterClassExA(&wc)) {
        MessageBoxA(nullptr, "Не вдалося зареєструвати клас вiкна", "Помилка", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowExA(
        0,
        "GraphicsWndClass",
        "Основнi функцiї базової графiки (WinAPI)",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT,
        840, 600,
        nullptr, nullptr, g_hInst, nullptr);

    if (!hwnd) {
        MessageBoxA(nullptr, "Не вдалося створити вiкно", "Помилка", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
