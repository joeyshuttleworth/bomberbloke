#ifndef ABSTRACTHUDElEMENT_HPP
#define ABSTRACTHUDElEMENT_HPP

#include <SDL2/SDL.h>

#include "engine.hpp"

class Camera;
extern SDL_Renderer *_renderer;
SDL_Texture* get_sprite(std::string);

enum AlignFlag {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
    ALIGN_BOTTOM,
    ALIGN_TOP
};

class AbstractHudElement {
public:
    /**
     * If set to false, onInput will not be called by scene.
     */
    bool mIsInteractive = false;

     /**
      * @brief Base class constructor for HUD
      *
      * If xAlignFlag is set to ALIGN_LEFT the bounding box will be positioned with
      * its left side xPos pixels away from the left side of the window. If
      * its set to ALIGN_RIGHT it will be positioned with its right side xPos pixels away
      * from the right side of the window. Setting it to ALIGN_CENTER will centre align the
      * box in the x-axis (regardless of the position values). yAlignFlag works
      * similarly with ALIGN_TOP corresponding to alignment relative to the top of the
      * window and ALIGN_BOTTOM corresponding to the bottom.
      *
      * @param xPos - the position of the hud element in pixels
      * @param yPos - the position of the hud element in pixels
      * @param xDim - give the dimensions of the bounding box of the element in px
      * @param yDim - give the dimensions of the bounding box of the element in px
      * @param xAlignFlag - AlignFlag determine the alignment of the bounding box.
      * @param yAlignFlag  - determine the alignment of the bounding box.
      */
    AbstractHudElement(int xPos, int yPos, int xDim, int yDim,
                       AlignFlag xAlignFlag=ALIGN_LEFT, AlignFlag yAlignFlag = ALIGN_BOTTOM) {
        // Actual position is set in updatePosition
        mRelativePosition[0] = xPos;
        mRelativePosition[1] = yPos;
        
        mDimensions[0] = xDim;
        mDimensions[1] = yDim;
        
        mAlignFlags[0] = xAlignFlag;
        mAlignFlags[1] = yAlignFlag;
    }

    /**
     * Draws a grey box corresponding to the bounding box.
     * Called by scene on every frame. updatePosition must be called once at
     * some point before it is drawn.
     */
  virtual void draw(std::shared_ptr<Camera> camera) {
        SDL_Rect dstRect;
        dstRect.x = mPosition[0];
        dstRect.y = mPosition[1];
        dstRect.w = mDimensions[0];
        dstRect.h = mDimensions[1];
        
        SDL_SetRenderDrawColor(_renderer, 0xa0, 0xa0, 0xa0, 0xff);
        SDL_RenderFillRect(_renderer, &dstRect);
        
        return;
    }
    
    /**
     * Converts the relative position, dimension and alignment flags into the
     * actual position of the bounding box from its top left corner. This must
     * be called at least once before draw is called.
     */
    void updatePosition(std::shared_ptr<Camera> camera) {
        std::array<int, 2> screenDimensions = camera->getScreenDimensions();
        
        switch(mAlignFlags[0]) {
            case ALIGN_CENTER:
                // Centred positioning
                mPosition[0] = (screenDimensions[0] - mDimensions[0]) / 2;
                break;
            case ALIGN_RIGHT:
                // Right-aligned positioning
                mPosition[0] = screenDimensions[0] - mDimensions[0] - mRelativePosition[0];
                break;
            default:
                // Left-aligned positioning
                mPosition[0] = mRelativePosition[0];
        }
        
        switch(mAlignFlags[1]) {
            case ALIGN_CENTER:
                // Centred positioning
                mPosition[1] = (screenDimensions[1] - mDimensions[1]) / 2;
                break;
            case ALIGN_RIGHT:
                // Right-aligned positioning
                mPosition[1] = screenDimensions[1] - mDimensions[1] - mRelativePosition[1];
                break;
            default:
                // Left-aligned positioning
                mPosition[1] = mRelativePosition[1];
        }
    }

    /**
     * Update method to be called on every tick.
     */
    virtual void update() {};
    
    /**
     * Called by scene whenever the engine detects user input.
     */
    virtual void onInput(SDL_Event *event) {};
    
protected:
    /**
     * Pixel-position of the top left corner of the bounding box.
     */
    int mPosition[2];
    
    /**
     * Pixel-dimensions of the bounding box containing the element.
     */
    int mDimensions[2];

    /**
     * Relative positional values (see xPos and yPos in the constructor) that
     * are converted to actual positional values in updatePosition based on the
     * parameters of the current camera and the alignment flags.
     */
    int mRelativePosition[2];
    
    /**
     * x and y alignment flags (see constructor).
     */
    int mAlignFlags[2];
};

#endif
