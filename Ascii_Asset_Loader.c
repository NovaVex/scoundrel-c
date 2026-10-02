// ========================================================
// Ascii_Asset_Loader.c
// --------------------------------------------------------
// Loads ASCII art from a category text file into memory,
// and frees it again. One file holds many assets; each one
// is a name line, a width and height line, then its rows.
//
// A short or malformed file stops the read. Whatever loaded
// cleanly before that point is kept and reported.
// ========================================================

#include "Ascii_Asset_Loader.h"
#include <stdio.h>
#include <stdlib.h>

// ========================================================
// Memory Loader
// ========================================================

// Steps over whatever is left on the current line, up to and including its newline.
void skipRestOfLine(FILE* file) {
    int character = fgetc(file);

    while (character != '\n' && character != EOF) {   // && means AND: keep going until the line ends or the file does
        character = fgetc(file);
    }
}

// Reads one asset's size line and its rows. Cleans up after itself and says false if the file runs short.
bool readAssetGrid(FILE* file, AsciiAsset* asset) {
    if (fscanf(file, "%d %d", &asset->width, &asset->height) != 2) return false;           // both numbers must be there
    if (asset->width <= 0 || asset->height <= 0) return false;                             // || means OR: a grid with no width, or none with no height

    skipRestOfLine(file);                                                                  // land on the art itself. A "\n" in the format above would eat its leading spaces

    asset->data = malloc(asset->height * sizeof(char*));
    if (asset->data == NULL) return false;                                                 // If the machine had no memory to give

    for (int row = 0; row < asset->height; row++) {
        asset->data[row] = malloc((asset->width + ASSET_ROW_PADDING) * sizeof(char));

        if (asset->data[row] == NULL) {                                                    // If this row's memory couldn't be had
            freeAssetRows(asset, row);                                                     // only the rows before this one were built
            return false;
        }

        if (fgets(asset->data[row], asset->width + ASSET_ROW_PADDING, file) == NULL) {     // If the file ended part way through the art
            freeAssetRows(asset, row + 1);                                                 // this row was built too, so free it as well
            return false;
        }
    }

    return true;
}

// Reads a category file into a new array of assets, and reports how many were found.
AsciiAsset* loadAssetCategory(const char* filepath, int* assetCount) {
    *assetCount = 0;

    FILE* file = fopen(filepath, "r");
    if (file == NULL) return NULL;   // If the file isn't there, there is nothing to load

    AsciiAsset* categoryList = malloc(MAX_ASSETS * sizeof(AsciiAsset));

    if (categoryList == NULL) {      // If the machine had no memory for the array
        fclose(file);
        return NULL;
    }

    // && means AND: there has to be room left in the array, and a name still to read.
    // A name that isn't there is how the file ends.
    while (*assetCount < MAX_ASSETS && fscanf(file, "%31s", categoryList[*assetCount].name) == 1) {   // 31 must stay one less than ASSET_NAME_SIZE
        AsciiAsset* current = &categoryList[*assetCount];

        if (!readAssetGrid(file, current)) break;   // If this asset didn't read cleanly, stop and keep the ones before it

        (*assetCount)++;
    }

    fclose(file);

    return categoryList;
}

// ========================================================
// Memory Sweeper
// ========================================================

// Frees the first rowsBuilt rows of one asset, then its row list.
void freeAssetRows(AsciiAsset* asset, int rowsBuilt) {
    if (asset->data == NULL) return;   // If this asset never got as far as having rows

    for (int row = 0; row < rowsBuilt; row++) {
        free(asset->data[row]);
    }

    free(asset->data);
    asset->data = NULL;
}

// Frees every asset in the array, then the array itself.
void freeAssetCategory(AsciiAsset* categoryList, int assetCount) {
    if (categoryList == NULL) return;   // If the load failed, there is no array to free

    for (int assetIndex = 0; assetIndex < assetCount; assetIndex++) {
        AsciiAsset* current = &categoryList[assetIndex];

        freeAssetRows(current, current->height);
    }

    free(categoryList);
}
