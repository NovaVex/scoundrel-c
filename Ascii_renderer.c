// ========================================================
// Ascii_renderer.c
// --------------------------------------------------------
// Draws a frame off-screen, then pushes it to the terminal
// in one go, so nothing flickers while it is being built.
//
// Clear the buffer, stamp assets onto it, then present it.
// ========================================================

#include "Ascii_Asset_Loader.h"
#include <stdio.h>

//Screen Definitions
#define SCREEN_WIDTH 120
#define SCREEN_HEIGHT 40
#define FRAME_OUTPUT_SIZE ((SCREEN_WIDTH + 1) * SCREEN_HEIGHT + 1)   // every row, plus its \n, plus the \0 that ends the text

// ========================================================
// The Frame Buffer
// ========================================================

// The back buffer: the exact characters the next frame will show.
typedef struct FrameBuffer {
    char pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
} FrameBuffer;

// ========================================================
// Clear the Buffer
// ========================================================

// Blanks the canvas so last frame's characters don't linger.
void clearBuffer(FrameBuffer* buffer) {
    for (int row = 0; row < SCREEN_HEIGHT; row++) {
        for (int column = 0; column < SCREEN_WIDTH; column++) {
            buffer->pixels[row][column] = ' ';
        }
    }
}

// ========================================================
// Blitting: Stamp an Asset onto the Buffer
// ========================================================

// Copies an asset's characters into the buffer at the given corner.
void drawAssetToBuffer(FrameBuffer* buffer, AsciiAsset* asset, int startX, int startY) {
    if (asset == NULL || asset->data == NULL) return;   // || means OR: no asset, or no rows in it

    for (int row = 0; row < asset->height; row++) {
        for (int column = 0; column < asset->width; column++) {
            char pixel = asset->data[row][column];

            // Spaces are see-through, so an asset doesn't rub out what's behind it.
            if (pixel == ' ' || pixel == '\n' || pixel == '\0') continue;   // || means OR: a space, a line ending, or the end of the row

            int targetX = startX + column;
            int targetY = startY + row;

            // Anything hanging off the edge of the screen is clipped, not written.
            // && means AND: all four have to be true for the character to land on the screen.
            if (targetX >= 0 && targetX < SCREEN_WIDTH && targetY >= 0 && targetY < SCREEN_HEIGHT) {
                buffer->pixels[targetY][targetX] = pixel;
            }
        }
    }
}

// ========================================================
// Present the Buffer
// ========================================================

// Flattens the buffer into one string and writes the whole frame at once.
void presentBuffer(FrameBuffer* buffer) {
    printf("\033[H");   // cursor back to the top-left, without clearing: the old frame is overwritten, not blanked

    char outputString[FRAME_OUTPUT_SIZE];
    int cursor = 0;

    for (int row = 0; row < SCREEN_HEIGHT; row++) {
        for (int column = 0; column < SCREEN_WIDTH; column++) {
            outputString[cursor++] = buffer->pixels[row][column];
        }

        outputString[cursor++] = '\n';
    }

    outputString[cursor] = '\0';

    fputs(outputString, stdout);
    fflush(stdout);
}
