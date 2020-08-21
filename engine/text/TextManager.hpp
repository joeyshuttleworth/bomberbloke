#ifndef TEXTMANAGER_HPP
#define TEXTMANAGER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "Text.hpp"

class TextManager {
public:
    TextManager() {
        if (TTF_Init() == -1) {
            std::cout << "TTF_Init: " << TTF_GetError() << std::endl;
        }
    }

    ~TextManager() {
        if (TTF_WasInit()) {
            TTF_Quit();
        }
    }

    void loadFontFromPath(std::string path, std::string id, int ptSize=16) {
        TTF_Font font = TTF_OpenFont(path.c_str(), ptSize);
        
        if (font) {
            mFontBank.insert(std::make_pair(id, font));
        } else {
            std::cout << "TTF_OpenFont: " << TTF_GetError() << std::endl;
        }
    }
    
    std::shared_ptr<Text> createText(std::string fontId) {
        std::shared_ptr<Text> newText = std::make_shared<Text>(
            mFontBank[fontId]
        );
        
        return newText;
    }
private:
    std::map<std::string, TTF_Font*> mFontBank;
};

#endif