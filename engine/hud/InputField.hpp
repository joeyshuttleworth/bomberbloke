#ifndef INPUTFIELD_HPP
#define INPUTFIELD_HPP

#include "ClickableHudElement.hpp"
#include "TextHudElement.hpp"

class InputField: public ClickableHudElement, public TextHudElement {
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
    InputField(std::shared_ptr<Text> text, int xPos, int yPos, int xDim,
        int yDim, void (*onClickFn)()=nullptr, AlignFlag xAlignFlag=ALIGN_LEFT,
        AlignFlag yAlignFlag=ALIGN_TOP);

    /**
     * Sets the colour of text on mouse over.
     *
     * @param colour    New colour object.
     */
    void setMouseOverColour(SDL_Color colour) {
        mMouseOverColour = colour;
        mPropertiesUpdated = true;
    }

    void setRectangleColour(SDL_Color colour) {
      mMouseOverColour = colour;
      mPropertiesUpdated = true;
    }


    /**
     * Sets the colour of text on left-click.
     *
     * @param colour    New colour object.
     */
    void setOnClickColour(SDL_Color colour) {
        mOnClickColour = colour;
        mPropertiesUpdated = true;
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
        mPropertiesUpdated = true;
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
        mPropertiesUpdated = true;
    }

    /**
     * Draws the text object in the position of the HUD element.
     *
     * @param camera    Current Camera object.
     */
    void draw(Camera *camera) override;

    void onInput(SDL_Event *event) override;

protected:
    // Text colour when neither a mouse-over or left-click is detected.
    SDL_Color mDefaultColour;
    // Text colour when a mouse over is detected.
    SDL_Color mMouseOverColour;
    // Text colour when a click is detected.
    SDL_Color mOnClickColour;

    SDL_Color mBackgroundColour;

    // Text offset when neither a mouse-over or left-click is detected.
    int mDefaultOffset[2] = { 0, 0 };
    // Text offset when a mouse over is detected.
    int mMouseOverOffset[2] = { 0, 0 };
    // Text offset when a click is detected.
    int mOnClickOffset[2] = { 0, 0 };

    std::string textInput = "";
};

#endif
