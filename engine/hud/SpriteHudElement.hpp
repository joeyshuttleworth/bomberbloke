#ifndef SPRITEHUDELEMENT_HPP
#define SPRITEHUDELEMENT_HPP

#include "AbstractHudElement.hpp"

#include <string>
#include "SDL2/SDL_image.h"

class Camera;
SDL_Texture* get_sprite(std::string);

class SpriteHudElement: public virtual AbstractHudElement {
public:
    /**
     * Constructor for SpriteHudElement.
     *
     * See constructor for AbstractHudElement for more info on positioning
     * and alignment.
     *
     * @param xPos          The position of the hud element in px.
     * @param yPos          The position of the hud element in px.
     * @param xDim          The dimensions of the bounding box of the element in px.
     * @param yDim          The dimensions of the bounding box of the element in px.
     * @param xAlignFlag    Determines the alignment of the bounding box.
     * @param yAlignFlag    DetermineS the alignment of the bounding box.
     */
   SpriteHudElement(std::string asset, int xPos, int yPos, int xDim, int yDim,
           AlignFlag xAlignFlag=ALIGN_LEFT, AlignFlag yAlignFlag=ALIGN_TOP);
    /**
     * Draw function.
     *
     * Called by scene on every frame. Any children classes that overide
     * this function must call it before drawing.
     *
     * @param camera    Current Camera object.
     */
    void draw(Camera* camera);

    /**
     * Sets amount of glow applied to the sprite during post-processing.
     *
     * @param glowAmount    Intensity of glow - 0-255 where 0 is no glow.
     */
    void setGlowAmount(int glowAmount) {
        mGlowAmount = glowAmount;
        mPropertiesUpdated = true;
    }

  virtual ~SpriteHudElement(){}

protected:
    // Sprite rendered onto HUD bounding box.
    SDL_Texture *mSprite;

    // Amount of glow applied in post-processing - 0-255
    int mGlowAmount = 0;
};

#endif
