#ifndef ABSTRACTPLAYERPROPERTIES_HPP
#define ABSTRACTPLAYERPROPERTIES_HPP

class AbstractPlayerProperties{
public:
  //virtual void SetProperty() = 0;
  virtual std::vector<std::string> GetProperties(){return {};}
  virtual void reset(std::shared_ptr<AbstractPlayerProperties> p_properties = nullptr) = 0;
};

#endif
