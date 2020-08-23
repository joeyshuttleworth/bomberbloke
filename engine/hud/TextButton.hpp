#ifndef TEXTBUTTON_HPP
#define TEXTBUTTON_HPP

#include "ClickableHudElement.hpp"
#include "TextHudElement.hpp"

class TextButton: public ClickableHudElement, public TextHudElement {
public:
    /**
     * Constructor for TextButton.
     *
     * Calls base class consructor and stores text object.
     *
     * @param text          Text object that is rendered in HUD element.
     * @param xPos          The position of the hud element in px.
     * @param yPos          The position of the hud element in px.
     * @param xDim          The dimensions of the bounding box of the element in px.
     * @param yDim          The dimensions of the bounding box of the element in px.
     * @param onClickFn     The static function that is called when the element
     *                      is left-clicked by the mouse.
     * @param xAlignFlag    Determines the alignment of the bounding box.
     * @param yAlignFlag    DetermineS the alignment of the bounding box.
     */
    TextButton(std::shared_ptr<Text> text, int xPos, int yPos, int xDim,
        int yDim, void (*onClickFn)()=nullptr, AlignFlag xAlignFlag=ALIGN_LEFT,
        AlignFlag yAlignFlag=ALIGN_TOP);
    
    /**
     * Sets the colour of text on mouse over.
     *
     * @param red   The red component.
     * @param green The green component.
     * @param blue  The blue component.
     * @param alpha The alpha component.
     */
    void setMouseOverColour(int red, int green, int blue, int alpha=0) {
        mMouseOverColour.r = red % 256;
        mMouseOverColour.g = green % 256;
        mMouseOverColour.b = blue % 256;
        mMouseOverColour.a = alpha % 256;
    }
    
    /**
     * Sets the colour of text on left-click.
     *
     * @param red   The red component.
     * @param green The green component.
     * @param blue  The blue component.
     * @param alpha The alpha component.
     */
    void setOnClickColour(int red, int green, int blue, int alpha=0) {
        mOnClickColour.r = red % 256;
        mOnClickColour.g = green % 256;
        mOnClickColour.b = blue % 256;
        mOnClickColour.a = alpha % 256;
    }
    
    /**
     * Sets the offset of the text on mouse-over.
     *
     * @param xOffset   X-coordinate of the offset (pixels).
     * @param yOffset   Y-coordinate of the offset (pixels).
     */
    void setMouseOverOffset(int xOffset, int yOffset) {
        mMouseOverOffset[0] = xOffset;
        mMouseOverOffset[1] = yOffset;
    }
    
    /**
     * Sets the offset of the text on left-click.
     *
     * @param xOffset   X-coordinate of the offset (pixels).
     * @param yOffset   Y-coordinate of the offset (pixels).
     */
    void setOnClickOffset(int xOffset, int yOffset) {
        mOnClickOffset[0] = xOffset;
        mOnClickOffset[1] = yOffset;
    }
    
    /**
     * Draws the text object in the position of the HUD element.
     * 
     * @param camera    Current Camera object.
     */
    void draw(Camera *camera);

protected:
    // Text colour when neither a mouse-over or left-click is detected.
    SDL_Color mDefaultColour;
    // Text colour when a mouse over is detected.
    SDL_Color mMouseOverColour;
    // Text colour when a click is detected.
    SDL_Color mOnClickColour;
    
    // Text offset when neither a mouse-over or left-click is detected.
    int mDefaultOffset[2] = { 0, 0 };
    // Text offset when a mouse over is detected.
    int mMouseOverOffset[2] = { 0, 0 };
    // Text offset when a click is detected.
    int mOnClickOffset[2] = { 0, 0 };
};

#endif