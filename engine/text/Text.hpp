#ifndef TEXT_HPP
#define TEXT_HPP

#include <SDL2/SDL_ttf.h>
#include <string>

class Camera;
extern SDL_Renderer *_renderer;

// Enum for text alignment (see setTextAlignment).
enum TextAlignFlag {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
    ALIGN_BOTTOM,
    ALIGN_TOP
};

/**
 * Class for rendering text.
 *
 * Create text objects using TextManager.
 */
class Text {
public:
    /**
     * Constructor for Text.
     * 
     * Refer to TextManager for how to create Text objects and load fonts.
     *
     * @param font  TTF font used to render the text
     */
    Text(TTF_Font *font) {
        mFont = font;
        mPropertiesUpdated = true;
    }
    
    /**
     * Sets the string that is rendered.
     *
     * @param text  String that is rendered on draw.
     */
    void setText(std::string text) {
        mTextString = text;
        mPropertiesUpdated = true;
    }
    
    /**
     * Sets the screen position of the text box.
     *
     * The position corresponds to the top-left corner of the text box. The
     * coordinates are given in number of pixels where the the top-left corner
     * of the screen is (0, 0).
     *
     * @param xPos  X-coordinate of the screen position (pixels).
     * @param yPos  Y-coordinate of the screen position (pixels).
     */
    void setPosition(int xPos, int yPos) {
        mTextBox.x = xPos;
        mTextBox.y = yPos;
        mPropertiesUpdated = true;
    }
    
    /**
     * Sets the offset of the text.
     *
     * Offset is the number pixels in each direction that the text is shifted
     * following alignment.
     *
     * @param xOffset   X-coordinate of the offset (pixels).
     * @param yOffset   Y-coordinate of the offset (pixels).
     */
    void setTextOffset(int xOffset, int yOffset) {
        mOffset[0] = xOffset;
        mOffset[1] = yOffset;
        mPropertiesUpdated = true;
    }
    
    /**
     * Sets the alignment of the text.
     *
     * Determines the horizontal and vertical alignment of the text.
     *
     * @param xAlign    Horizontal alignment of the text. Accepts ALIGN_LEFT,
     *                  ALIGN_RIGHT and ALIGN_CENTER (default).
     * @param yAlign    Horizontal alignment of the text. Accepts ALIGN_TOP,
     *                  ALIGN_BOTTOM and ALIGN_CENTER (default).
     */
    void setTextAlignment(TextAlignFlag xAlign, TextAlignFlag yAlign=ALIGN_TOP) {
        mAlignment[0] = xAlign;
        mAlignment[1] = yAlign;
    }
    
    /**
     * Sets the colour of the text.
     *
     * Sets RGBA colour values using integers in the range 0-255.
     *
     * @param red   The red component.
     * @param green The green component.
     * @param blue  The blue component.
     * @param alpha The alpha component.
     */
    void setTextColour(int red, int green, int blue, int alpha=255) {
        mColour.r = red % 256;
        mColour.g = green % 256;
        mColour.b = blue % 256;
        mColour.a = alpha % 256;
    }
    
    /**
     * Sets the dimensions of the text box.
     *
     * Sets the size (in pixels) of the text box.
     *
     * @param xDim    Number of pixels wide.
     * @param yDim    Number of pixels high.
     */
    void setDimensions(int xDim, int yDim) {
        mSrcRect.w = xDim;
        mSrcRect.h = yDim;
        mTextBox.w = xDim;
        mTextBox.h = yDim;
        mPropertiesUpdated = true;
    }
    
    /**
     * Draws the text to the renderer.
     *
     * @param camera    Current scene camera.
     */
    void draw(Camera *camera);

protected:
    // Text font given on construction (typically by the text manager).
    TTF_Font *mFont;
    
    // String that is rendered.
    std::string mTextString;
    // Source rectangle.
    SDL_Rect mSrcRect;
    // Destination rectangle - corresponds to the text box in screen coordinates.
    SDL_Rect mTextBox;
    // Offset values (see setOffset).
    int mOffset[2];
    // Colour of rendered text.
    SDL_Color mColour;
    // Text alignment flags in x and y direction (see setTextAlignment).
    TextAlignFlag mAlignment[2];

    // Texture containing rendered text. Updated only when mPropertiesUpdated
    // is set to True (see draw).
    SDL_Texture *mTextTexture;
    // Boolean value which is set to true whenever a property is changed that
    // may effect the render. Set back to false when draw is called.
    bool mPropertiesUpdated;
};

#endif