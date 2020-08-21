#ifndef TEXTMANAGER_HPP
#define TEXTMANAGER_HPP

#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>

class Text;

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
     */
    std::shared_ptr<Text> createText(std::string fontId);

private:
    // Map from font ID (see loadFontFromPath) to SDL font object.
    std::map<std::string, TTF_Font*> mFontBank;
};

#endif