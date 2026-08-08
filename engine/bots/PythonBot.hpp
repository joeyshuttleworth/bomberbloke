#ifndef PYTHONBOT_HPP
#define PYTHONBOT_HPP

#include <fstream>
#include "AbstractBot.hpp"
#include "BotAPI.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;


class __attribute__((visibility("hidden"))) PythonBot : public AbstractBot{
protected:
  Uint8 mState = 0;
  unsigned int mPollDelay = 120;

  int getActorId();

  const std::string mPythonModuleName;

  BotAPI mBotAPI;

  std::unique_ptr<py::object> mpPyBot;

public:
  virtual ~PythonBot();

  PythonBot(std::string nickname, std::shared_ptr<scene> scn, std::shared_ptr<actor> a,
            std::unique_ptr<NetClient> n_client,
            const std::string python_module_name);

  void handleReset() override;

  void update(unsigned int) override;

  using AbstractBot::AbstractBot;
  using AbstractBot::operator=;
};


#endif
