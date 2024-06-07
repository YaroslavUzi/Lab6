#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "Shape.h"

#pragma comment(lib, "comctl32.lib")

#define N 102

Shape* pcshape[N] = { nullptr };
int shapeCount = 0;
POINT startPoint, endPoint;
bool isDrawing = false;
Shape* currentShape = nullptr;
int currentObject = 0;
HWND hToolbar = NULL;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void CreateToolbar(HWND hwnd);
void StartDrawing(HWND hwnd, POINT point);
void UpdateDrawing(HWND hwnd, POINT point);
void EndDrawing(HWND hwnd, POINT point);
void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    InitCommonControls();

    wincl.hInstance = hInst;
    wincl.lpszClassName = TEXT("Lab6Class");
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);

    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

    if (!RegisterClassEx(&wincl))
        return 0;

    hwnd = CreateWindowEx(
        0,
        TEXT("Lab6Class"),
        TEXT("Lab6"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        544,
        375,
        HWND_DESKTOP,
        NULL,
        hInst,
        NULL
    );

    ShowWindow(hwnd, ncmdshow);

    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

void CreateToolbar(HWND hwnd)
{
    TBBUTTON tbb[4];
    TBADDBITMAP tbab;

    hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0,
        hwnd, (HMENU)IDC_TOOLBAR, GetModuleHandle(NULL), NULL);

    SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(hToolbar, TB_ADDBITMAP, 0, (LPARAM)&tbab);

    ZeroMemory(tbb, sizeof(tbb));

    tbb[0].iBitmap = STD_FILENEW;
    tbb[0].fsState = TBSTATE_ENABLED;
    tbb[0].fsStyle = TBSTYLE_BUTTON;
    tbb[0].idCommand = ID_TOOL_POINT;

    tbb[1].iBitmap = STD_FILEOPEN;
    tbb[1].fsState = TBSTATE_ENABLED;
    tbb[1].fsStyle = TBSTYLE_BUTTON;
    tbb[1].idCommand = ID_TOOL_LINE;

    tbb[2].iBitmap = STD_FILESAVE;
    tbb[2].fsState = TBSTATE_ENABLED;
    tbb[2].fsStyle = TBSTYLE_BUTTON;
    tbb[2].idCommand = ID_TOOL_RECTANGLE;

    tbb[3].iBitmap = STD_PRINT;
    tbb[3].fsState = TBSTATE_ENABLED;
    tbb[3].fsStyle = TBSTYLE_BUTTON;
    tbb[3].idCommand = ID_TOOL_ELLIPSE;

    SendMessage(hToolbar, TB_ADDBUTTONS, 4, (LPARAM)&tbb);
}

void StartDrawing(HWND hwnd, POINT point) {
    isDrawing = true;
    startPoint = point;

    switch (currentObject) {
    case IDM_POINT:
        currentShape = new PointShape();
        break;
    case IDM_LINE:
        currentShape = new LineShape();
        break;
    case IDM_RECTANGLE:
        currentShape = new RectangleShape();
        break;
    case IDM_ELLIPSE:
        currentShape = new EllipseShape();
        break;
    }

    currentShape->SetData(startPoint, startPoint);
}

void UpdateDrawing(HWND hwnd, POINT point) {
    endPoint = point;
    currentShape->SetData(startPoint, endPoint);
    InvalidateRect(hwnd, NULL, TRUE);
}

void EndDrawing(HWND hwnd, POINT point) {
    isDrawing = false;
    endPoint = point;
    currentShape->SetData(startPoint, endPoint);

    if (shapeCount < N) {
        pcshape[shapeCount++] = currentShape;
    }
    else {
        delete currentShape;
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;

    if (pnmh->code == TTN_NEEDTEXT) {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom) {
        case ID_TOOL_POINT:
            lstrcpy(lpttt->szText, L"Крапка");
            break;
        case ID_TOOL_LINE:
            lstrcpy(lpttt->szText, L"Лінія");
            break;
        case ID_TOOL_RECTANGLE:
            lstrcpy(lpttt->szText, L"Прямокутник");
            break;
        case ID_TOOL_ELLIPSE:
            lstrcpy(lpttt->szText, L"Еліпс");
            break;
        default:
            lstrcpy(lpttt->szText, L"Невідомо");
        }
    }
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_CREATE:
        CreateToolbar(hwnd);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_POINT:
        case ID_TOOL_POINT:
            currentObject = IDM_POINT;
            SetWindowText(hwnd, TEXT("Lab6 - Крапка"));
            break;

        case IDM_LINE:
        case ID_TOOL_LINE:
            currentObject = IDM_LINE;
            SetWindowText(hwnd, TEXT("Lab6 - Лінія"));
            break;

        case IDM_RECTANGLE:
        case ID_TOOL_RECTANGLE:
            currentObject = IDM_RECTANGLE;
            SetWindowText(hwnd, TEXT("Lab6 - Прямокутник"));
            break;

        case IDM_ELLIPSE:
        case ID_TOOL_ELLIPSE:
            currentObject = IDM_ELLIPSE;
            SetWindowText(hwnd, TEXT("Lab6 - Еліпс"));
            break;

        case IDM_EXIT:
            PostQuitMessage(0);
            break;

        case IDM_ABOUT:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, [](HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) -> INT_PTR {
                if (message == WM_COMMAND)
                {
                    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
                    {
                        EndDialog(hDlg, LOWORD(wParam));
                        return (INT_PTR)TRUE;
                    }
                }
                return (INT_PTR)FALSE;
                });
            break;
        }
        break;

    case WM_LBUTTONDOWN:
        StartDrawing(hwnd, { LOWORD(lParam), HIWORD(lParam) });
        break;

    case WM_MOUSEMOVE:
        if (isDrawing) {
            UpdateDrawing(hwnd, { LOWORD(lParam), HIWORD(lParam) });
        }
        break;

    case WM_LBUTTONUP:
        EndDrawing(hwnd, { LOWORD(lParam), HIWORD(lParam) });
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        for (int i = 0; i < shapeCount; ++i) {
            pcshape[i]->Draw(hdc);
        }

        if (isDrawing && currentShape) {
            currentShape->Draw(hdc);
        }

        EndPaint(hwnd, &ps);
    }
    break;

    case WM_NOTIFY:
        OnNotify(hwnd, wParam, lParam);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
