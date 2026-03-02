#include "assets.hpp"
#include <SDL_image.h>
#include <fstream>

/*
* assets.cpp
* Desktop build system embeds assets in the final executable, web browser uses the emscripten
* toolchain to bundle them together.
*/

#ifndef __EMSCRIPTEN__
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(files);
#endif

void
loadAssets(TextManager&, //textManager,
           SoundManager&, // soundManager,
           IGraphicsManager& graphicsManager)
{
    // Collect assets
    std::vector<std::string> filenames;
    std::ifstream f("assets/web_assets.txt");
    std::string remove = "assets/";
    std::string line;
    while (std::getline(f,line)) {
      line.replace(line.begin(), line.begin() + remove.size(), "");
      filenames.push_back(line);
    }

    // Load
    for(auto &entry : filenames) {
        auto dot_pos = entry.find('.');
        // if (dot_pos == std::string::npos)
        //     continue; // no file extension

        std::string file_name = entry.substr(0, dot_pos);
        std::string file_extension = entry.substr(dot_pos);
        // std::string full_path = "assets/" + entry;
        // SDL_RWops *io = SDL_RWFromFile(full_path.c_str(), "rb");

        if (file_extension == ".ttf")
        {
        // textManager.loadFontFromPath(io, file_name);
        }
        else if (file_extension == ".ogg")
        {
        // soundManager.loadFromPath(io, file_name);
        }
        else if (file_extension == ".png"){
          graphicsManager.loadSpriteFromPath(entry);
        }
    }
}
