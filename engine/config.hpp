#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "engine.hpp"
#include <fstream>
#include <list>

bool LoadConfig(std::string fname);
bool ExecFile(std::string fname);
bool GenerateConfig(std::string fname);

#endif
