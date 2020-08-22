#ifndef ABSTRACTHUDElEMENT_HPP
#define ABSTRACTHUDElEMENT_HPP

#include "engine.hpp"

class Camera;
extern SDL_Renderer *_renderer;

enum AlignFlag {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
    ALIGN_BOTTOM,
    ALIGN_TOP
};

/**
 * Abstract class for HUD elements that are stored, updated and drawn in Scene.
 */
class AbstractHudElement {
public:
    /**
     * If set to false, onInput will not be called by scene.
     */
    bool mIsInteractive = false;

     /**
      * Base class constructor for HUD.
      *
      * If xAlignFlag is set to ALIGN_LEFT the bounding box will be positioned with
      * its left side xPos pixels away from the left side of the window. If
      * its set to ALIGN_RIGHT it will be positioned with its right side xPos pixels away
      * from the right side of the window. Setting it to ALIGN_CENTER will centre align the
      * box in the x-axis (regardless of the position values). yAlignFlag works
      * similarly with ALIGN_TOP corresponding to alignment relative to the top of the
      * window and ALIGN_BOTTOM corresponding to the bottom.
      *
      * @param xPos          The position of the hud element in px.
      * @param yPos          The position of the hud element in px.
      * @param xDim          The dimensions of the bounding box of the element in px.
      * @param yDim          The dimensions of the bounding box of the element in px.
      * @param xAlignFlag    Determines the alignment of the bounding box.
      * @param yAlignFlag    DetermineS the alignment of the bounding box.
      */
    AbstractHudElement(int xPos, int yPos, int xDim, int yDim,
            AlignFlag xAlignFlag=ALIGN_LEFT, AlignFlag yAlignFlag=ALIGN_TOP);
    
    /**
     * Sets the (relative) screen position of the bounding box.
     *
     * The position corresponds to the top-left corner of the box. See the
     * constructor for information on how this property interacts with
     * alignment.
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
     * Sets the dimensions of the bounding box.
     *
     * Sets the size (in pixels) of the bounding box.
     *
     * @param xDim  Number of pixels wide.
     * @param yDim  Number of pixels high.
     */
    void setDimensions(int xDim, int yDim) {
        mDimensions[0] = xDim;
        mDimensions[1] = yDim;
        mPropertiesUpdated = true;
    }
    
    /**
     * Sets the alignment of the bounding box. (see constructor)
     *
     * @param xAlign    Horizontal alignment of the box. Accepts ALIGN_LEFT,
     *                  ALIGN_RIGHT and ALIGN_CENTER (default).
     * @param yAlign    Horizontal alignment of the box. Accepts ALIGN_TOP,
     *                  ALIGN_BOTTOM and ALIGN_CENTER (default).
     */
    void setTextAlignment(AlignFlag xAlign, AlignFlag yAlign=ALIGN_TOP) {
        mAlignFlags[0] = xAlign;
        mAlignFlags[1] = yAlign;
    }

    /**
     * Draws a grey box corresponding to the bounding box.
     *
     * Called by scene on every frame. updatePosition must be called once at
     * some point before it is drawn.
     * 
     * @param camera    Current Camera object.
     */
    virtual void draw(Camera* camera);
    
    /**
     * Updates the position of the HUD element.
     *
     * Converts the relative position, dimension and alignment flags into the
     * actual position of the bounding box from its top left corner. This must
     * be called at least once before draw is called.
     * 
     * @param camera    Current Camera object.
     */
    virtual void updatePosition(Camera* camera);

    /**
     * Update method to be called on every tick.
     */
    virtual void update() {};
    
    /**
     * Called by scene whenever the engine detects user input.
     *
     * @param event Input event that is handled by HUD element.
     */
    virtual void onInput(SDL_Event *event) {};
    
protected:
    // Pixel-position of the top left corner of the bounding box.
    int mPosition[2];
    
    // Pixel-dimensions of the bounding box containing the element.
    int mDimensions[2];

    // Relative positional values (see xPos and yPos in the constructor) that
    // are converted to actual positional values in updatePosition based on the
    // parameters of the current camera and the alignment flags.
    int mRelativePosition[2];
    
    // x and y alignment flags (see constructor).
    int mAlignFlags[2];
    
    // Boolean value which is set to true whenever a property is changed that
    // may effect the render. Set back to false when draw is called.
    bool mPropertiesUpdated;
};

#endif
