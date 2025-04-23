#include <windows.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <cmath>
#include <vector>

#include "flag.h" // Include the flag design
#include "blaster.h"  // Include the gun design

const int WIDTH = 800;  // Window width
const int HEIGHT = 600; // Window height
const int FRAME_DELAY = 1000 / 60; // 60 FPS

// Bubble structure
struct Bubble {
    int x, y;
    float velocityX, velocityY;
    bool active;
    float pullDistance; // Distance the bubble is pulled back by the pulley effect
    bool pullingBack;   // Flag to determine if bubble is being pulled back
    float maxPullDistance; // Maximum distance the bubble can be pulled
    float floatVelocityY;  // Velocity for floating upwards
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
        // Draw a bubble (a circle)
        Ellipse(hdc, bubble.x - 15, bubble.y - 15, bubble.x + 15, bubble.y + 15);
    }
}

// Update bubble's position based on physics and the pulley effect
void UpdateBubble(Bubble &bubble) {
    if (bubble.pullingBack) {
        // Simulate pulling back effect (gradually increase pull distance)
        if (bubble.pullDistance < bubble.maxPullDistance) {
            bubble.pullDistance += 1.0f; // Adjust speed of pulling
        }
    } else {
        // Apply the opposite force (bubble travels away from the character)
        bubble.x += bubble.velocityX;
        bubble.y += bubble.velocityY;

        // Apply upward float velocity
        if (bubble.velocityY < 0) {
            bubble.velocityY += 0.1f; // Slow down the rise due to gravity-like effect
        }

        // Simulate gravity pulling the bubble down (after float slows)
        if (bubble.velocityY >= 0) {
            bubble.velocityY -= 0.5f; // Gravity pulling downwards
        }

        // If bubble goes out of the screen, deactivate it
        if (bubble.x < 0 || bubble.x > WIDTH || bubble.y > HEIGHT) {
            bubble.active = false;
        }
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

    // List to store active bubbles
    std::vector<Bubble> bubbles;

    // Initialize bubble properties
    float maxPullDistance = 100.0f;
    float floatVelocityY = -5.0f; // Initial upward velocity for bubble

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

        // Handle bubble creation when spacebar is held down
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            // Create a new bubble
            Bubble newBubble = { charX - 40, charY - 5, 0.0f, 0.0f, true, 0.0f, true, maxPullDistance };
            bubbles.push_back(newBubble); // Add bubble to the list
        }

        // Update each bubble's position
        for (auto& bubble : bubbles) {
            // If the bubble is being pulled back, update its position accordingly
            if (bubble.pullingBack) {
                if (bubble.pullDistance < bubble.maxPullDistance) {
                    bubble.pullDistance += 1.0f; // Increase pull distance
                }
                bubble.x = charX - bubble.pullDistance; // Update position relative to pull
            } else {
                // Update bubble's position (after release)
                bubble.x += bubble.velocityX;
                bubble.y += bubble.velocityY;

                // Apply floating and gravity effects
                if (bubble.velocityY < 0) {
                    bubble.velocityY += 0.1f;
                }
                if (bubble.velocityY >= 0) {
                    bubble.velocityY -= 0.5f;
                }
            }
        }

        // Stop pulling back and launch bubbles when spacebar is released
        if (!(GetAsyncKeyState(VK_SPACE) & 0x8000)) {
            for (auto& bubble : bubbles) {
                if (bubble.pullingBack) {
                    bubble.pullingBack = false;
                    bubble.velocityX = 5.0f * (bubble.pullDistance / maxPullDistance);  // Launch speed
                    bubble.velocityY = floatVelocityY;  // Launch upwards
                }
            }
        }

        // Clear screen by filling the background with white
        RECT rect = { 0, 0, WIDTH, HEIGHT };
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Draw character
        DrawCharacter(hdc, charX, charY);
        
        // Draw the flag and blaster
        DrawFlag(hdc, charX + 30, charY-50); // Adjusted left side
        DrawBlaster(hdc, charX - 35, charY); // Adjusted left side

        // Draw each active bubble
        for (auto& bubble : bubbles) {
            DrawBubble(hdc, bubble);
        }

        // Capture the current frame data
        std::string frameData = "Frame " + std::to_string(frameCount++) + ": "
                                "Character at (" + std::to_string(charX) + ", " + std::to_string(charY) + "), "
                                "Bubbles active: " + std::to_string(bubbles.size());
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
