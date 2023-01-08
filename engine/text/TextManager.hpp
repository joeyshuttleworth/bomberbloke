#ifndef TEXTMANAGER_HPP
#define TEXTMANAGER_HPP

#include "SDL_ttf.h"
#include <memory>
#include <string>
#include <map>

#include "Text.hpp"

/**
 * Loads and stores fonts and creates Text objects.
 */
class TextManager {
public:
    /**
     * Constructor for TextManager.
     */
    TextManager();

    /**
     * Destructor for TextManager.
     */
    ~TextManager();

    /**
     * Loads .ttf font file.
     *
     * @param path      String containing path to .ttf file.
     * @param id        String that will be used to refer to the font when
     *                  creating Text objects.
     * @param ptSize    Point-size of font that is loaded.
     */
    void loadFontFromPath(std::string path, std::string id, int ptSize=16);

    /**
     * Creates a text object given a specified font.
     *
     * Font must first be loaded using loadFontFromPath.
     *
     * @param fontId    ID corresponding to font (see loadFontFromPath).
     * @param text  String that is rendered.
     * @param xPos  X-coordinate of the screen position (pixels).
     * @param yPos  Y-coordinate of the screen position (pixels).
     * @param xDim    Number of pixels wide.
     * @param yDim    Number of pixels high.
     * @param xAlign    Horizontal alignment of the text. Accepts TEXT_ALIGN_LEFT,
     *                  TEXT_ALIGN_RIGHT and TEXT_ALIGN_CENTER (default).
     * @param yAlign    Horizontal alignment of the text. Accepts TEXT_ALIGN_TOP,
     *                  TEXT_ALIGN_BOTTOM and TEXT_ALIGN_CENTER (default).
     */
    std::shared_ptr<Text> createText(std::string fontId, std::string text="",
        int posX=0, int posY=0, int xDim=0, int yDim=0,
        TextAlignFlag xTextAlign=TEXT_ALIGN_LEFT,
        TextAlignFlag yTextAlign=TEXT_ALIGN_TOP);

private:
    // Map from font ID (see loadFontFromPath) to SDL font object.
    std::map<std::string, TTF_Font*> mFontBank;
};

#endif
