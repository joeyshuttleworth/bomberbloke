#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

void
load_config(std::string fname)
{
  std::ifstream fin(fname, std::ifstream::in);
  std::string buf;
  int player_id = 0;
  bool in_player = false;
  if (!fin.good())
    return;
  while (buf = fin.getline()) {
    std::transform(data.begin(), data.end(), ::tolower);
    if (in_player) {
      if (buf.find("end player")) {
        in_player = false;
        player_id++;
      } else {
        common_binding tmp_binding;
        std::string tmp;

        _player_list.back.control_scheme.push_back(tmp_binding);
      }
    } else {
      if (buf.find("start player")) {
        _player_list.push_back(local_p("noob"));
        in_player = true;
      }
    }
  }
}

std::string
remove_extra_whitespace(std::string str)
{
  bool space = true;
  std::string rc;
  for (char& ch : str) {
    if (isspace(ch) && ch != ' ')
      continue;
    else if (space) {
      if (ch != ' ') {
        rc += ch;
      } else {
        space = false;
        rc += ch;
      }
    }
  }
  return;
}
