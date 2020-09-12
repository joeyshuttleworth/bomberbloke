#ifndef CLICKABLEHUDElEMENT_HPP
#define CLICKABLEHUDElEMENT_HPP

#include "AbstractHudElement.hpp"

#include <functional>

class ClickableHudElement: public virtual AbstractHudElement {
public:
    /**
     * Constructor for an abstract clickable HUD element - calls the base
     * constructor.
     *
     * @param xPos          The position of the hud element in px.
     * @param yPos          The position of the hud element in px.
     * @param xDim          The dimensions of the bounding box of the element in px.
     * @param yDim          The dimensions of the bounding box of the element in px.
     * @param onClickFn     The static function that is called when the element
     *                      is left-clicked by the mouse.
     * @param xAlignFlag    Determines the alignment of the bounding box.
     * @param yAlignFlag    DetermineS the alignment of the bounding box.
     */
    ClickableHudElement(int xPos, int yPos, int xDim, int yDim,
        std::function<void()> onClickFn, AlignFlag xAlignFlag=ALIGN_LEFT,
        AlignFlag yAlignFlag=ALIGN_BOTTOM);

    /**
     * Called by the handle input function.
     *
     * @param camera    Current Camera object.
     */
    void onInput(SDL_Event *event);

    // TODO: support keyboard and joystick selection

protected:
    /**
     * The static function that is called when the element is left-clicked by
     * the mouse.
     */
    std::function<void()> mOnClickFn;

    /**
     * Boolean that is set to true when the element is left-clicked. It is set
     * back to false when the click is released.
     */
    bool mIsClicked = false;
    /**
     * Boolean that is set to true only when the cursor is hovering over the
     * element.
     */
    bool mIsMouseOver = false;

private:
    /**
     * Method that returns true only if the pixel-coordinates (x, y) are
     * contained within the element's bounding box
     */
    bool isCoordOnElement(int x, int y);
};

#endif
