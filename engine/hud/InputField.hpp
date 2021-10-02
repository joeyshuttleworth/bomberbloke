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
        int yDim, AlignFlag xAlignFlag=ALIGN_LEFT, AlignFlag yAlignFlag=ALIGN_TOP);

    /**
     * Sets the colour of text after input is given.
     *
     * @param colour    New colour object.
     */
    void setInputColour(SDL_Color colour) {
        mInputColour = colour;
        mPropertiesUpdated = true;
    }

    /**
     * Sets the text input string.
     *
     * @param newText New text string.
     */
    void setInputText(std::string newText) {
        mTextInput = newText;
        if(mText)
          mText->setText(mTextInput);
        mPropertiesUpdated = true;
    }

    /**
     * Update method to be called on every tick.
     */
    void update() override;

    /**
     * Called by the handle input function.
     *
     * @param camera    Current Camera object.
     */
    void onInput(SDL_Event *event) override;

    /**
     * Draws the text object in the position of the HUD element.
     *
     * @param camera    Current Camera object.
     */
    void draw(Camera *camera) override;

protected:
    // Text colour of default text for when input field is empty
    SDL_Color mDefaultColour;
    // Text colour when input is given.
    SDL_Color mInputColour;

    // Current text in field
    std::string mTextInput = "";
    // Text shown when input field is empty
    std::string mDefaultText = "";

    // Boolean value if the box has current focus
    bool mHasFocus = false;

    // Cursor index
    int mCursorIndex = 0;

    /**
     * Function that is called on click.
     */
    void onClick(int x, int y) override;
};

#endif
