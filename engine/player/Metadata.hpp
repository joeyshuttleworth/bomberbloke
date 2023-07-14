#ifndef METADATA_HPP
#define METADATA_HPP

#include <string>
#include <map>
#include <cereal/archives/json.hpp>


struct Metadata {
  std::map<std::string, std::string> character;
  std::map<std::string, int> numeric;

  Metadata() {};

  void applyTo(Metadata &m) {
    for(auto const &c : character)
      m.character[c.first] = c.second;
    for(auto const &n : numeric)
      m.numeric[n.first] = n.second;
  }

  template<class Archive>
  void serialize(Archive & archive)
  {
    archive( character, numeric );
  }
};

#endif
