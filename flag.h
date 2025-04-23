#include <windows.h>

// Function to draw a flag pointing up
void DrawFlag(HDC hdc, int x, int y) {
    // Draw the pole (black)
    RECT pole = { x, y, x + 10, y + 100 };  // Width: 10, Height: 100
    FillRect(hdc, &pole, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // Draw the flag (red) pointing up from the top of the pole
    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    RECT flag = { x + 10, y, x + 60, y + 30 };  // Width: 50, Height: 30 (right of the pole)
    FillRect(hdc, &flag, redBrush);
    DeleteObject(redBrush);
}
