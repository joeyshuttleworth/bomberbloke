#include "assets.hpp"
#include <SDL_image.h>

/*
* assets.cpp
* Desktop build system embeds assets in the final executable, web browser uses the emscripten
* toolchain to bundle them together.
*/

#ifdef __EMSCRIPTEN__

void
loadAssets(TextManager& textManager,
           SoundManager& soundManager,
           SpriteList& spriteList)
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
        if (dot_pos == std::string::npos)
            continue; // no file extension

        std::string file_name = entry.substr(0, dot_pos);
        std::string file_extension = entry.substr(dot_pos);
        std::string full_path = "assets/" + entry;
        SDL_RWops *io = SDL_RWFromFile(full_path.c_str(), "rb");

        if (file_extension == ".ttf")
        {
        textManager.loadFontFromPath(io, file_name);
        }
        else if (file_extension == ".ogg")
        {
        soundManager.loadFromPath(io, file_name);
        }
        else if (file_extension == ".png"){
        SDL_Texture* sprite = IMG_LoadTexture_RW(_renderer, io, 1);
        spriteList.push_back({ entry, sprite });
        }
    }
}

#else

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(files);

void
loadAssets(TextManager& textManager,
           SoundManager& soundManager,
           SpriteList& spriteList)
{
  auto fs = cmrc::files::get_filesystem();
  for (auto &&entry : fs.iterate_directory("files/assets/"))
  {
    auto dot_pos = entry.filename().find('.');
    if (dot_pos == std::string::npos)
    {
      continue;
    } // no file extension

    std::string file_name = entry.filename().substr(0, dot_pos);
    std::string file_extension = entry.filename().substr(dot_pos);

    auto file = fs.open("files/assets/" + entry.filename());
    SDL_RWops *io = SDL_RWFromConstMem(file.begin(), file.end() - file.begin());

    if (file_extension == ".ttf")
    {
      textManager.loadFontFromPath(io, file_name);
    }
    else if (file_extension == ".ogg")
    {
      soundManager.loadFromPath(io, file_name);
    }
    else if (file_extension == ".png"){
      SDL_Texture* sprite = IMG_LoadTexture_RW(_renderer, io, 1);
      spriteList.push_back({ entry.filename(), sprite });
    }
  }

}

#endif