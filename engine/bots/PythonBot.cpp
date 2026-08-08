#include <random>
#include "LocalPlayer.hpp"
#include "PythonBot.hpp"
#include "CommandEvent.hpp"
#include "serverPlayer.hpp"
#include "actor.hpp"
#include "BotAPI.hpp"
#include <pybind11/embed.h>

PYBIND11_EMBEDDED_MODULE(bloke, m)
{
  py::class_<BotAPI>(m, "Bot")
    .def("handleCommand", &BotAPI::handleCommand)
    .def("getSceneJSON", &BotAPI::getSceneJSON)
    .def("getActors", &BotAPI::getActors);
}

PythonBot::PythonBot(std::string nickname, std::shared_ptr<scene> s,
                     std::shared_ptr<actor> a,
                     std::unique_ptr<NetClient> n_client,
                     const std::string module_name):
  AbstractBot::AbstractBot(nickname, s, a, std::move(n_client)),
  mPythonModuleName(module_name),
  mBotAPI(this)
{
  try
    {
      py::module sys = py::module::import("sys");
      sys.attr("path").attr("insert")(0, "./python");
      py::module botModule = py::module::import(mPythonModuleName.c_str());
      mpPyBot = std::make_unique<py::object>(botModule.attr("PythonBot")());
    }
  catch (const py::error_already_set &e)
    {
      log_message(ERR, e.what());
    }
}

void PythonBot::handleReset()
{
  mState = 0;

  if(!mpPyBot)
    return;

  try
    {
      if (!mpPyBot->is_none())
        mpPyBot->attr("handleReset")();
    }
  catch (const py::error_already_set &e)
    {
      log_message(ERR, e.what());
    }
}

void PythonBot::update(unsigned int current_tick){
  if (current_tick - mLastHandledTick < mPollDelay)
    return;

  if(!mpPyBot)
    return;

  mLastHandledTick = current_tick;

  try
    {
      if (!mpPyBot->is_none())
        mpPyBot->attr("update")(py::cast(&mBotAPI), current_tick);
    }
  catch (const py::error_already_set &e)
    {
      log_message(ERR, e.what());
    }
}

PythonBot::~PythonBot(){}

