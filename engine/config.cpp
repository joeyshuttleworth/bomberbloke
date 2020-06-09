#include "config.hpp"
#include <iostream>

bool ExecFile(std::string fname){
  std::ifstream config_file(fname);
  std::string tmp_line;
  
  int line_no = 0;
  while(std::getline(config_file, tmp_line)){
    line_no++;
    if(handle_system_command(split_to_tokens(tmp_line))){
      log_message(ERROR, "error executing file at " + fname + ":" + std::to_string(line_no) + ". The rest of the file was not read." );
    };
  }
}

bool LoadConfig(std::string fname = "default.config"){
  log_message(DEBUG, "Reading config file " + fname +"." );
  /*TODO: Here we should default all settings and remove all command bindings*/
  ExecFile(fname);
}

/*
  Produces a list of commands which reproduce the current settings.
  A comprehensive list should be found here (but it isn't).
*/

bool GenerateConfig(std::string fname){
std::ofstream config_file(fname, std::ios::out);

  /*First, add key bindings*/
  LocalPlayer player = _local_player_list.front();

  for(auto i = player.mControlScheme.begin(); i != player.mControlScheme.end(); i++){
config_file << "bind " <<   i->command  << " " <<i->scancode<< "\n";
  }
  /*TODO: add every single command which could go in a config file*/
}
