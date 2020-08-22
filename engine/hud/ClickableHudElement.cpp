#include "ClickableHudElement.hpp"

ClickableHudElement::ClickableHudElement(int xPos, int yPos, int xDim, int yDim,
        void (*onClickFn)(), AlignFlag xAlignFlag, AlignFlag yAlignFlag)
        : AbstractHudElement(xPos, yPos, xDim, yDim, xAlignFlag, yAlignFlag) {
    // Base class constructor must be called
    mOnClickFn = onClickFn;
    
    // Set to be interactive so that onInput is called by scene
    mIsInteractive = true;
}

bool ClickableHudElement::isCoordOnElement(int x, int y) {
    if (x >= mPosition[0] && x <= mPosition[0] + mDimensions[0]) {
        if (y >= mPosition[1] && y <= mPosition[1] + mDimensions[1])
            return true;
    }    
    return false;
}

void ClickableHudElement::onInput(SDL_Event *event) {
    // If its a mouse button up event, it cannot be clicked
    if (mIsClicked && event->type == SDL_MOUSEBUTTONUP) {
        mIsClicked = false;
        return;
    }
    
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        // If it is a mouse button down event and the cursor is on the button
        // it must be clicked and the mouse must be over it
        if (isCoordOnElement(event->button.x, event->button.y)) {
            mIsMouseOver = true;
            mIsClicked = true;
            if (mOnClickFn != nullptr)
                mOnClickFn();
        } else {
            mIsMouseOver = false;
        }
    } else if (event->type == SDL_MOUSEMOTION) {
        // If it is a mouse motion event check if the cursor is on the button
        mIsMouseOver = isCoordOnElement(event->motion.x, event->motion.y);
    }
}