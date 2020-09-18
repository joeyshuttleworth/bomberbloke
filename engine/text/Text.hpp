#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include <array>
#include <SDL2/SDL_ttf.h>

class Camera;
extern SDL_Renderer *_renderer;

// Enum for text alignment (see setTextAlignment).
enum TextAlignFlag {
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT,
    TEXT_ALIGN_BOTTOM,
    TEXT_ALIGN_TOP
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
     * @param font  TTF font used to render the text.
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
    Text(TTF_Font *font, std::string text="", int posX=0, int posY=0,
            int xDim = 0, int yDim=0, TextAlignFlag xAlign=TEXT_ALIGN_LEFT,
            TextAlignFlag yAlign=TEXT_ALIGN_TOP) {
        mFont = font;
        mTextString = text;
        mPosition[0] = posX;
        mPosition[1] = posY;
        mDimensions[0] = xDim;
        mDimensions[1] = yDim;
        mAlignment[0] = xAlign;
        mAlignment[1] = yAlign;
        mPropertiesUpdated = true;

        mColour = SDL_Color({255, 255, 255, 255});
        mBackColour = SDL_Color({0, 0, 0, 0});
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
        mPosition[0] = xPos;
        mPosition[1] = yPos;
        mPropertiesUpdated = true;
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
        mDimensions[0] = xDim;
        mDimensions[1] = yDim;
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
     * Gets the offset of the text.
     *
     * @return  Array containing offset in x and y directions (pixels).
     */
    std::array<int, 2> getTextOffset() {
        return { mOffset[0], mOffset[1] };
    }

    /**
     * Sets the scale of the text.
     *
     * @param xScale    Scale parameter in the x-direction.
     * @param yScale    Scale parameter in the y-direction. If left blank it is
     *                  set to the same value as xScale.
     */
    void setTextScale(double xScale, double yScale=-1.) {
        mTextScale[0] = xScale;
        if (xScale >= 0) {
            mTextScale[0] = xScale;

            if (yScale >= 0)
                mTextScale[1] = yScale;
            else
                mTextScale[1] = mTextScale[0];
        }

        mPropertiesUpdated = true;
    }

    /**
     * Sets the alignment of the text.
     *
     * Determines the horizontal and vertical alignment of the text.
     *
     * @param xAlign    Horizontal alignment of the text. Accepts TEXT_ALIGN_LEFT,
     *                  TEXT_ALIGN_RIGHT and TEXT_ALIGN_CENTER (default).
     * @param yAlign    Horizontal alignment of the text. Accepts TEXT_ALIGN_TOP,
     *                  TEXT_ALIGN_BOTTOM and TEXT_ALIGN_CENTER (default).
     */
    void setTextAlignment(TextAlignFlag xAlign, TextAlignFlag yAlign=TEXT_ALIGN_TOP) {
        mAlignment[0] = xAlign;
        mAlignment[1] = yAlign;

        mPropertiesUpdated = true;
    }

    /**
     * Sets the colour of the text.
     *
     * @param colour    The new colour.
     */
    void setTextColour(SDL_Color colour) {
        mColour = colour;
        mPropertiesUpdated = true;
    }

    /**
    * Sets the colour of the background.
    *
    * @param colour    The new colour.
    */
    void setBackgroundColour(SDL_Color colour) {
        mBackColour = colour;
        mPropertiesUpdated = true;
    }


    /**
     * Gets the text colour.
     *
     * @return  SDL_Color object.
     */
    SDL_Color getTextColour() {
        return mColour;
    }

    /**
     * Sets the ammount of glow applied to the text.
     *
     * @param glowAmount  New glow amount.
     */
    void setGlowAmount(int glowAmount) {
        mGlowAmount = glowAmount;
        mPropertiesUpdated = true;
    }

    /**
     * Draws the text to the renderer.
     *
     * @param camera            Current scene camera.
     * @param isPostProcessed   Determines whether the text is drawn after
     *                          camera post-processing.
     */
    void draw(Camera *camera, bool isPostProcessed=true);

protected:
    // Text font given on construction (typically by the text manager).
    TTF_Font *mFont;

    // String that is rendered.
    std::string mTextString;
    // Pixel-position of the top left corner of the bounding box.
    int mPosition[2];
    // Pixel-dimensions of the bounding box containing the element.
    int mDimensions[2];
    // Offset values (see setOffset).
    int mOffset[2] = { 0, 0 };
    // Ratio by which text is scaled.
    double mTextScale[2] = { 1., 1. };
    // Colour of rendered text.
    SDL_Color mColour;
    // Colour of background colour;
    SDL_Color mBackColour;
    // Text alignment flags in x and y direction (see setTextAlignment).
    TextAlignFlag mAlignment[2];
    // Amount of glow applied (0-255).
    int mGlowAmount = 0;

    // Texture containing rendered text. Updated only when mPropertiesUpdated
    // is set to True (see draw).
    SDL_Texture *mTextTexture;
    // Source rectangle.
    SDL_Rect mSrcRect;
    // Destination rectangle - corresponds to the text box in screen coordinates.
    SDL_Rect mDstRect;
    // Boolean value which is set to true whenever a property is changed that
    // may effect the render. Set back to false when draw is called.
    bool mPropertiesUpdated;
    /**
     * Updates the texture used in the draw function.
     *
     * @param camera    Current scene camera.
     */
    void updateTexture(Camera *camera);
};

#endif
