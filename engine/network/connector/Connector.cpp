#include <Connector.hpp>
#include <engine.hpp>

Connector::~Connector()
{
  log_message(DEBUG, "Connector - destructor called");
}
