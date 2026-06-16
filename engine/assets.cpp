#include "assets.hpp"

#include "IGraphicsManager.hpp"
#include "ISoundManager.hpp"

/*
* assets.cpp
* Desktop build system embeds assets in the final executable, web browser uses the emscripten
* toolchain to bundle them together.
*/

#ifdef __EMSCRIPTEN__

void
loadAssets(
           SoundManager& soundManager,
           IGraphicsManager& graphicsManager)
{
    // Collect assets
    std::vector<std::string> filenames;
    std::ifstream f("assets/web_assets.txt");
    std::string remove = "assets/";
    std::string line;
    while (std::getline(f, line)) {
      line.replace(line.begin(), line.begin() + remove.size(), "");
      filenames.push_back(line);
    }

    // Load
    for(auto &entry : filenames) {
        auto dot_pos = entry.find('.');
        if (dot_pos == std::string::npos)
            continue; // no file extension

        if (file_extension == ".ogg")
        {
          soundManager.loadFromPath(entry);
        }
        else if (file_extension == ".png"){
          graphicsManager.loadSpriteFromPath(entry);
        }
    }
}

#else
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(files);

void loadAssets(
                ISoundManager& soundManager,
                IGraphicsManager& graphicsManager)
{
  auto fs = cmrc::files::get_filesystem();

  const std::string dir = "files/assets";

  for (auto &&entry : fs.iterate_directory(dir))
    {
      auto dot_pos = entry.filename().find('.');
      if (dot_pos == std::string::npos)
        {
          continue;
        }

      std::string file_name = entry.filename().substr(0, dot_pos);
      std::string file_extension = entry.filename().substr(dot_pos);

      auto fname = entry.filename();

      if (file_extension == ".ogg")
        {
          soundManager.loadFromPath(dir + "/" + entry.filename(), file_name);
        }
      else if (file_extension == ".png"){
        graphicsManager.loadSpriteFromPath(fname);
      }
    }

}

#endif
