#include <random>
#include "LocalPlayer.hpp"
#include "PythonBot.hpp"
#include "CommandEvent.hpp"
#include "serverPlayer.hpp"
#include "actor.hpp"
#include "BomberBlokeBotAPI.hpp"
#include "BomberBlokePythonBot.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>

PYBIND11_EMBEDDED_MODULE(bloke, m)
{
  py::class_<BotAPI>(m, "Bot")
    .def("handleCommand", &BotAPI::handleCommand)
    .def("getSceneJSON", &BotAPI::getSceneJSON)
    .def("getNavGrid", &BotAPI::getNavGrid)
    .def("getConnectedComponentFromNode", &BotAPI::getConnectedComponentFromNode)
    .def("getActorLocation", &BotAPI::getActorLocation);
  py::class_<BomberBlokeBotAPI, BotAPI>(m, "BomberBlokeBot")
    .def("getDangerousSquares", &BomberBlokeBotAPI::getDangerousSquares);
  py::class_<NavGrid>(m, "NavGrid")
    // This is necessary for the python types to work nicely (can't do Set[List])
    .def("getNodes", [](NavGrid& self) {
      py::set result;
      for (auto node : self.getNodes()) {
        result.add(py::make_tuple(node[0], node[1]));
      }
      return result;
    });
}

