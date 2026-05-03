#ifndef ABSTRACTCAMERA_HPP
#define ABSTRACTCAMERA_HPP

class scene;
class IGraphicsManager;

class AbstractCamera{
protected:
  scene* mpScene = nullptr;
  IGraphicsManager& mrGraphicsManager;

public:
  AbstractCamera(IGraphicsManager& gfx,
                 scene* scn=nullptr) : mrGraphicsManager(gfx)
  {
    mpScene = scn;
  };

  virtual ~AbstractCamera(){};

  virtual void resetFrameBuffers(){};
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

  scene* getScene(){
    return mpScene;
  }

  IGraphicsManager& getGraphicsManager(){return mrGraphicsManager;};
};

#endif
