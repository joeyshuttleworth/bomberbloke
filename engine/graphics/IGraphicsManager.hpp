#ifndef IGRAPHICSMANAGER_HPP
#define IGRAPHICSMANAGER_HPP

#include<string>
#include<array>
#include<map>
#include<memory>
#include "AbstractTexture.hpp"

class Text;
class scene;

using Rect = std::array<int, 4>;

class IGraphicsManager{
public:

  // Allows for SDL like function calls
  virtual void renderClear(){};

  virtual void resizeWindow(int=0, int=0){};
  virtual void renderSplashScreen(){};
  virtual void setDraw(bool){};
  virtual void drawScreen(){};
  virtual void loadSpriteFromPath(std::string){};
  virtual void applyBloom(double, double, int=1, AbstractTexture* =nullptr, AbstractTexture* = nullptr){};
  virtual void applyBlur(double, int){};
  virtual void applyBrightness(double){};
  virtual void drawNoProcessingBuffer(){};
  virtual void resetFrameBuffers(){};
  virtual void drawSprite(std::string, Rect, bool=true, int=0, AbstractTexture* = nullptr){};
  virtual void createWindow(int=-1, int=-1){};

  virtual AbstractTexture* getSprite(std::string){return nullptr;};

  virtual std::array<int, 2> getScreenDimensions(){return std::array<int, 2>{0, 0};};

  virtual void renderFillRect(Rect&, uint32_t,
                              bool=true, int=0, AbstractTexture* = nullptr){};

  virtual AbstractTexture* renderSolidText(std::string, int, std::string, uint32_t, AbstractTexture* =nullptr){return nullptr;};

  virtual void loadFont(std::string, int=12){};
  virtual void setBrightness(int){};

  virtual bool isWindowFullScreen(){return false;};
  virtual void setWindowFullScreen(bool=true){};

  virtual void destroyWindow(){};

  virtual void renderCopy(AbstractTexture*, Rect*, Rect*, bool=true, int=0,
                          AbstractTexture* =nullptr){};

  virtual void destroyTexture(AbstractTexture*){};

  virtual AbstractTexture* createTexture(int=0, int=0){return nullptr;};

  virtual std::array<int, 2> sizeText(std::string, int, std::string){return std::array<int ,2>{0, 0};};

  virtual std::shared_ptr<Text> createText(std::string="", std::string="Aileron-Black", int=12){return nullptr;};

  virtual ~IGraphicsManager();
};

#endif
