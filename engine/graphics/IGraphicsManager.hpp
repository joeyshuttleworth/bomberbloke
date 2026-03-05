#ifndef IGRAPHICSMANAGER_HPP
#define IGRAPHICSMANAGER_HPP
#include<string>
#include<array>
#include<map>

class scene;

class IGraphicsManager{
public:

  // Allows for SDL like function calls
  virtual void renderClear(){};

  virtual void resizeWindow(int=0, int=0){};

  virtual void init();

  virtual void renderSplashScreen(){};
  virtual void setDraw(bool){};
  virtual void drawScreen(){};
  virtual void loadSpriteFromPath(std::string){};
  virtual void applyBloom(double, double, int=1){};
  virtual void applyBlur(double, int){};
  virtual void applyBrightness(double){};
  virtual void drawNoProcessingBuffer(){};
  virtual void resetFrameBuffers(){};
  virtual void drawSprite(std::string, std::array<int, 4>, bool=true, int=0){};
  virtual void createWindow(){};

  virtual std::array<int, 2> getScreenDimensions(){return std::array<int, 2>{0, 0};};

  virtual void renderFillRect(std::array<int, 4>&, Uint32,
                              bool=true, int=0){};

  virtual void setBlur(double, int=0){};
  virtual void setBrightness(int){};
  virtual void setBloom(double, int=255, int=0){};
};

#endif
