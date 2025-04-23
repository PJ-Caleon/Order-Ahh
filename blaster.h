#include <windows.h>

// Function to draw a left-facing bubble gun
void DrawBlaster(HDC hdc, int x, int y) {
    HBRUSH blackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, blackBrush);

    // Gun body (main frame)
    Rectangle(hdc, x - 50, y, x, y + 20); // Width: 50, Height: 20 (left-facing)

    // Barrel (sticks out to the left)
    Rectangle(hdc, x - 65, y - 5, x - 40, y + 5); // Barrel flipped left

    // Sight (top back)
    Rectangle(hdc, x - 15, y - 8, x - 5, y - 3); // On top near the back

    // Trigger guard (circle)
    Ellipse(hdc, x - 25, y + 10, x - 15, y + 20); // Circle below gun

    // Handle (angled polygon, slanted down-left)
    POINT handle[] = {
        { x - 10, y + 20 },
        { x - 25, y + 20 },
        { x - 20, y + 40 },
        { x - 5,  y + 40 }
    };
    Polygon(hdc, handle, 4);

    // Restore brush
    SelectObject(hdc, oldBrush);
}
