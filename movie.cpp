#include <windows.h>
#include <chrono>
#include <thread>
#include <fstream>

#include "gun.h" // Include the gun design (we may keep gun visual for later)
#include "flag.h" // Include the flag design

const int WIDTH = 800;  // Window width
const int HEIGHT = 600; // Window height
const int FRAME_DELAY = 1000 / 60; // 60 FPS

// Bubble structure
struct Bubble {
    int x, y;
    bool active; // Track if the bubble is active
};

// Simple character drawer function
void DrawCharacter(HDC hdc, int x, int y) {
    // Draw the character body (a big circle)
    Ellipse(hdc, x - 30, y - 30, x + 30, y + 30);

    // Draw the character's eyes (two small circles)
    Ellipse(hdc, x - 25, y - 10, x - 15, y); 
    Ellipse(hdc, x - 10, y - 10, x, y); 
}

// Draw Bubble function
void DrawBubble(HDC hdc, Bubble &bubble) {
    if (bubble.active) {
        // Draw a small circle as the bubble
        Ellipse(hdc, bubble.x, bubble.y, bubble.x + 20, bubble.y + 20); // Larger to represent a bubble
    }
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;

    switch (msg) {
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Export function to save the animation frame data
void ExportAnimation(const std::string& frameData) {
    std::ofstream animFile("CoolMovie.anim", std::ios::app);
    if (animFile.is_open()) {
        animFile << frameData << std::endl;
        animFile.close();
    }
}

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // Register window class
    const char CLASS_NAME[] = "Cool Movie";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Create window
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Cool Movie", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);

    // Game timer setup
    auto startTime = std::chrono::high_resolution_clock::now();
    auto gameDuration = std::chrono::seconds(30); // Game lasts for 30 seconds

    // Initialize character's position and movement state
    int charX = WIDTH / 2;
    int charY = HEIGHT / 2;
    int moveSpeed = 5; // Speed of character movement

    // Bubble setup
    Bubble bubble = { -1, -1, false }; // Bubble starts off-screen

    // Create HDC
    HDC hdc = GetDC(hwnd);

    // Main game loop
    int frameCount = 0;
    while (true) {
        // Check if 30 seconds have passed
        auto elapsed = std::chrono::high_resolution_clock::now() - startTime;
        if (elapsed >= gameDuration) {
            break; // Exit game after 30 seconds
        }

        // Poll for messages and handle input
        MSG msg;
        if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Process key inputs
        if (GetAsyncKeyState('W') & 0x8000) charY -= moveSpeed;  // Move up
        if (GetAsyncKeyState('S') & 0x8000) charY += moveSpeed;  // Move down
        if (GetAsyncKeyState('A') & 0x8000) charX -= moveSpeed;  // Move left
        if (GetAsyncKeyState('D') & 0x8000) charX += moveSpeed;  // Move right

        // Blow bubble if spacebar is pressed
        if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !bubble.active) {
            bubble.x = charX - 10;  // Bubble starts from the center of the character
            bubble.y = charY - 5;
            bubble.active = true;
        }

        // Move the bubble to the left
        if (bubble.active) {
            bubble.x -= 10; // Bubble moves left now
            if (bubble.x < 0) {
                bubble.active = false;
            }
        }

        // Clear screen by filling the background with white
        RECT rect = { 0, 0, WIDTH, HEIGHT };
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Draw character
        DrawCharacter(hdc, charX, charY);
        
        // Draw the flag and gun 
        DrawFlag(hdc, charX + 30, charY-50); // Adjusted left side
        DrawGun(hdc, charX - 35, charY); // Adjusted left side (we may keep gun for visual consistency)

        // Draw bubble if active
        DrawBubble(hdc, bubble);

        // Capture the current frame data (as an example, we capture some details)
        std::string frameData = "Frame " + std::to_string(frameCount++) + ": "
                                "Character at (" + std::to_string(charX) + ", " + std::to_string(charY) + "), "
                                "Bubble active: " + std::to_string(bubble.active);
        ExportAnimation(frameData);

        // Wait for the next frame
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY));

        // Force window to refresh
        InvalidateRect(hwnd, nullptr, TRUE);
    }

    // Clean up and close the game
    PostQuitMessage(0);
    return 0;
}
