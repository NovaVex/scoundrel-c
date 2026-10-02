// ========================================================
// Ascii_Asset_Loader.h
// --------------------------------------------------------
// Blueprint for a piece of ASCII art and the two calls that
// bring a category file into memory and free it again.
// ========================================================

#pragma once
#include <stdbool.h>
#include <stdio.h>

//Asset Definitions
#define MAX_ASSETS 50
#define ASSET_NAME_SIZE 32     // 31 characters, plus 1 for the \0 that ends the text
#define ASSET_ROW_PADDING 2    // room on a row for the \n and the \0

// ========================================================
// Asset Data Structure
// ========================================================

// One piece of ASCII art: its name, its grid size, and its rows of text.
typedef struct AsciiAsset {
    char name[ASSET_NAME_SIZE];
    int width;
    int height;
    char** data;
} AsciiAsset;

// ========================================================
// Memory Loader
// ========================================================
void skipRestOfLine(FILE* file);
bool readAssetGrid(FILE* file, AsciiAsset* asset);
AsciiAsset* loadAssetCategory(const char* filepath, int* assetCount);

// ========================================================
// Memory Sweeper
// ========================================================
void freeAssetRows(AsciiAsset* asset, int rowsBuilt);
void freeAssetCategory(AsciiAsset* categoryList, int assetCount);
