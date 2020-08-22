#ifndef TEXTHUDElEMENT_HPP
#define TEXTHUDElEMENT_HPP

#include "AbstractHudElement.hpp"
#include "Text.hpp"

#include <memory>

class Camera;

class TextHudElement: public AbstractHudElement {
public:
    /**
     * Constructor for TextHudElement.
     *
     * Calls base class consructor and stores text object.
     *
     * @param text          Text object that is rendered in HUD element.
     * @param xPos          The position of the hud element in px.
     * @param yPos          The position of the hud element in px.
     * @param xDim          The dimensions of the bounding box of the element in px.
     * @param yDim          The dimensions of the bounding box of the element in px.
     * @param xAlignFlag    Determines the alignment of the bounding box.
     * @param yAlignFlag    DetermineS the alignment of the bounding box.
     */
    TextHudElement(std::shared_ptr<Text> text, int xPos, int yPos,
            int xDim, int yDim, AlignFlag xAlignFlag=ALIGN_LEFT,
            AlignFlag yAlignFlag=ALIGN_TOP);
    
    /**
     * Updates the position of the HUD element.
     *
     * Overides the virtual function in AbstarctHudElement, see
     * AbstarctHudElement.hpp for further information.
     * 
     * @param camera    Current Camera object.
     */
    void updatePosition(Camera* camera);
    
    /**
     * Draws the text object in the position of the HUD element.
     * 
     * @param camera    Current Camera object.
     */
    void draw(Camera *camera) {
        AbstractHudElement::draw(camera);
        mText->draw(camera);
    }
    
protected:
    // Text object used to render.
    std::shared_ptr<Text> mText;
};

#endif