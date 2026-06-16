#include "engine.hpp"


void
bot_loop()
{
  timespec t1, t2;
  t2.tv_nsec = 0;
  t2.tv_sec = 0;

  while (!_halt) {

    t1 = t2;
    // Sleep until next tick
    do{
      double time_to_sleep = (1e9/TICK_RATE - (t2.tv_nsec - t1.tv_nsec + 1e9 * (t2.tv_sec - t1.tv_sec)))*1e-6;
      if (clock_gettime(CLOCK_REALTIME, &t2) == -1)
        log_message(ERR, "Failed to get time");
      SDL_Delay(time_to_sleep*.8);
    }
    while(t2.tv_nsec - t1.tv_nsec +
          1e9 * (t2.tv_sec - t1.tv_sec) < 1e9 / TICK_RATE);

    // Perform client tick
    bot_entry();
  }
}

