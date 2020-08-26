#include "TextManager.hpp"

#include <SDL2/SDL.h>
#include <iostream>

TextManager::TextManager() {
    // Required initialisation for SDL_TTF.
    if (TTF_Init() == -1) {
        std::cout << "TTF_Init: " << TTF_GetError() << std::endl;
    }
}

TextManager::~TextManager() {
    // Destructor for SDL_TTF.
    if (TTF_WasInit()) {
        TTF_Quit();
    }
}

void TextManager::loadFontFromPath(std::string path, std::string id, int ptSize) {
    TTF_Font *font = TTF_OpenFont(path.c_str(), ptSize);

    // Adds font to font bank
    if (font) {
        mFontBank.insert(std::make_pair(id, font));
    } else {
        std::cout << "TTF_OpenFont: " << TTF_GetError() << std::endl;
    }
}

std::shared_ptr<Text> TextManager::createText(std::string fontId, std::string text,
        int posX, int posY, int xDim, int yDim, TextAlignFlag xTextAlign,
        TextAlignFlag yTextAlign) {
    // Obtain font
    TTF_Font *font = mFontBank[fontId];

    // Create a new Text object
    std::shared_ptr<Text> newText = std::make_shared<Text>(
        font, text, posX, posY, xDim, yDim, xTextAlign, yTextAlign
    );

    return newText;
}
