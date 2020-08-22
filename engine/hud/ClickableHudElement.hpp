#ifndef CLICKABLEHUDElEMENT_HPP
#define CLICKABLEHUDElEMENT_HPP

#include "AbstractHudElement.hpp"

class ClickableHudElement: public AbstractHudElement {
public:
    /**
     * Constructor for an abstract clickable HUD element - calls the base
     * constructor.
     * onClickFn is the static function that is called when the element is
     * left-clicked by the mouse.
     */
    ClickableHudElement(int xPos, int yPos, int xDim, int yDim,
        void (*onClickFn)()=nullptr, AlignFlag xAlignFlag=ALIGN_LEFT, AlignFlag yAlignFlag=ALIGN_BOTTOM);
        
    void onInput(SDL_Event *event);
    
    /**
     * Temporary overide for demo
     * TODO: remove
     */
    void draw(Camera* camera);
    
    // TODO: support keyboard and joystick selection

protected:
    /**
     * The static function that is called when the element is left-clicked by
     * the mouse.
     */
    void (*mOnClickFn)();
    
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