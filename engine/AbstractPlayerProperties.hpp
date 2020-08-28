#ifndef ABSTRACTPLAYERPROPERTIES_HPP
#define ABSTRACTPLAYERPROPERTIES_HPP

class AbstractPlayerProperties{
public:
  //virtual void SetProperty() = 0;
  /**
   * Returns a list of strings describing the properties. Intended to be useful
   * for debugging/logging.
   */
  virtual std::vector<std::string> GetProperties(){return {};}

  /**
   * Resets this objects properties. This is to be used by code in engine/
   * to set and send elements of the game state to players. For example,
   * in Bomberbloke, a derived class is defined in common/GamePlayerProperties.hpp
   * and says what power-ups a player has.
   *
   *
   * @param p_properties
   * An object derived from this class which the properties will be taken from
   * or a nullptr in which case the default constructor of the derived class will be used
   */
  virtual void reset(std::shared_ptr<AbstractPlayerProperties> p_properties = nullptr) = 0;
};

#endif
