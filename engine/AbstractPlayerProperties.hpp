#ifndef ABSTRACTPLAYERPROPERTIES_HPP
#define ABSTRACTPLAYERPROPERTIES_HPP

class AbstractPlayerProperties{
public:
  //virtual void SetProperty() = 0;
  virtual std::vector<std::string> GetProperties() = 0;
};

#endif
