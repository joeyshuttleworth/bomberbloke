#ifndef ABSTARCTHUDGROUP_HPP
#define ABSTARCTHUDGROUP_HPP

class AbstractHudGroup: public AbstractHudElement {
public:
  AbstractHudGroup(int xPos, int yPos): AbstractHudElement(xPos, yPos, 0, 0) {};

  /**
   * Adds hud element to group.
   *
   * @param hudElement  HUD element to be added to the group.
   */
  void addElement(std::shared_ptr<AbstractHudElement> hudElement) {
    mHudElements.push_back(hudElement);
    mPropertiesUpdated = true;
  }

  /**
   * Sets the position of the group.
   *
   * @param xPos  X-coordinate of the screen position (pixels).
   * @param yPos  Y-coordinate of the screen position (pixels).
   */
  void setPosition(int xPos, int yPos) override {
    mPosition[0] = xPos;
    mPosition[1] = yPos;

    int deltaPosition[2] = { xPos - mPosition[0], yPos - mPosition[1] };
    for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
      (*i)->setPosition(
        (*i)->getPosition()[0] + deltaPosition[0],
        (*i)->getPosition()[1] + deltaPosition[1]
      );
    }
    mPropertiesUpdated = true;
  }

  /**
   * Sets the mIsVisible boolean for the group.
   *
   * @param isVisible New mIsVisible value.
   */
  void setIsVisible(bool isVisible) override {
    mIsVisible = isVisible;
    mPropertiesUpdated = true;
  }

  /**
   * Sets the mIsPostprocessed boolean for the group's elements.
   *
   * @param isPostProcessed   New mIsPostProcessed value.
   */
  void setIsPostProcessed(bool isPostProcessed) override {
    mIsPostProcessed = isPostProcessed;
    for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
      (*i)->setIsPostProcessed(isPostProcessed);
    }
    mPropertiesUpdated = true;
  }

  /**
   * Draws the group's HUD elements.
   *
   * @param camera    Current Camera object.
   */
  void draw(Camera* camera) override {
    if (!mIsVisible)
      return;

    for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
      (*i)->draw(camera);
    }
  }

  /**
   * Updates the position of the HUD elements.
   *
   * @param camera    Current Camera object.
   */
  void updatePosition(Camera* camera) override {
    for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
      (*i)->updatePosition(camera);
    }
  }

  /**
   * Update method to be called on every tick.
   */
  void update() override {
    for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
      (*i)->update();
    }
  }

  /**
   * Called by scene whenever the engine detects user input.
   *
   * @param event Input event that is handled by HUD element.
   */
  void onInput(SDL_Event *event) override {
    if (!mIsInteractive)
      return;

    for (auto i = mHudElements.begin(); i != mHudElements.end(); i++) {
      (*i)->onInput(event);
    }
  }

protected:
  // List of HUD elements belonging to the group.
  std::list<std::shared_ptr<AbstractHudElement>> mHudElements;
};

#endif
