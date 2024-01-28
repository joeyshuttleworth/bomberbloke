#include <map>
#include <list>
#include <thread>
#include <memory>
#include <cassert>
#include <Connector.hpp>

#include <MessageEvent.hpp>
#include <JoinEvent.hpp>
#include <MoveEvent.hpp>
#include <QueryEvent.hpp>


std::list<std::shared_ptr<AbstractEvent>> sending;
std::list<std::shared_ptr<AbstractEvent>> recieved;


void run_server() {
  ConnectorDescriptor desc;
  desc.mode = SERVER;
  desc.listenPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Server started

  const auto start = std::chrono::steady_clock::now();
  const auto finish = start + std::chrono::seconds(2);
  while(std::chrono::steady_clock::now() < finish) {
    auto got = conn->poll(100);
    for(auto &rec : got)
       recieved.push_back(rec.event);
  }

  conn->close();
}

void run_client() {
  ConnectorDescriptor desc;
  desc.mode = CLIENT;
  desc.serverAddress = "localhost";
  desc.serverPort = 8888;

  auto conn = createConnector(desc);

  assert(conn->open()); // Connection succeeded

  for(auto &send : sending)
    conn->broadcastEvent(send);

  // Poll for 3 seconds
  conn->poll(3000);

  conn->close();
}


int
main(int, char**)
{
    // Queue some events
    sending.push_back(std::make_shared<MessageEvent>("message here"));
    sending.push_back(std::make_shared<MessageEvent>("another message"));
    sending.push_back(std::make_shared<MessageEvent>("one more"));
    sending.push_back(std::make_shared<JoinEvent>("nickname", 150));
    sending.push_back(std::make_shared<MoveEvent>());
    sending.push_back(std::make_shared<QueryEvent>());

    // Launch threads and send
    std::thread server(run_server);
    std::thread client(run_client);
    server.join();
    client.join();

    // Check we got them all back in any order
    auto collect = [](std::list<std::shared_ptr<AbstractEvent>> events){
        std::map<EventType, int> stats;
        for(auto &event : events)
            stats[event->getType()]++;
        for(auto &entry : stats)
            printf(" (%i: %i),", entry.first, entry.second);
        printf("\n");
        return stats;
    };

    auto sentStats = collect(sending);
    auto receieveStats = collect(recieved);
    assert(sentStats.size() == receieveStats.size());
    assert(std::equal(sentStats.begin(), sentStats.end(), receieveStats.begin()));

    return 0;
}

void new_game(std::string)
{
  return;
}
