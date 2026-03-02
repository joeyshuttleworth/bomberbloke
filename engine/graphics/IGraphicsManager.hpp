#ifndef IGRAPHICSMANAGER_HPP
#define IGRAPHICSMANAGER_HPP
#include<string>

class scene;

class IGraphicsManager{
public:

  // Allows for SDL like function calls
  virtual void renderClear();
  virtual void renderCopy();

  virtual void resizeWindow(int, int){};

  virtual void init();

  // Different interfaces will use different Camera types
  virtual void renderSplashScreen();
  virtual void setDraw(bool){};
  virtual void refreshSprites(){};
  virtual void drawScreen(){};
  virtual void loadSpriteFromPath(std::string){};
  virtual void applyBloom(double, int);
  virtual void applyBrightness(double);
  virtual void drawNoProcessingBuffer();
  virtual void resetFrameBuffers();
  virtual void blurTexture(SDL_Texture*, double, int);
};

#endif
