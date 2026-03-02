#ifndef ABSTRACTCAMERA_HPP
#define ABSTRACTCAMERA_HPP

class scene;
class IGraphicsManager;

class AbstractCamera{
protected:
  std::shared_ptr<scene> mpScene = nullptr;
  IGraphicsManager* mpGraphicsManager;

public:
  AbstractCamera(std::shared_ptr<IGraphicsManager>,
                 std::shared_ptr<scene>){};
  virtual ~AbstractCamera(){};

  virtual void resetFrameBuffer(){};
  virtual void renderCopy(){};
  virtual void renderFillRect(){};
  virtual void update(){};
  virtual void setBloom(){};
  virtual void setBrightness(){};
  virtual void blurTexture(){};
  virtual void getScreenRect(){};
  virtual void onResize(){};
  virtual void draw(){};
  virtual void setZoom(double){};
  virtual double getZoom(){return 0;};

  std::shared_ptr<scene> getScene(){
    return mpScene;
  }
};

#endif
