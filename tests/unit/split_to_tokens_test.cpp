#include "Explosion.hpp"
#include "bloke.hpp"
#include "bomb.hpp"
#include "bomberbloke.h"
#include "CommandQueue.hpp"
#include "cereal/archives/json.hpp"
#include <SDL2/SDL.h>

bool
compare_tokens(Tokens l1, Tokens l2)
{
  if (l1.size() != l2.size())
    return false;
  else {
    auto j = l2.begin();
    for (auto i = l1.begin(); i != l1.end(); i++) {
      if (*i != *j)
        return false;
      j++;
    }
  }
  return true;
}

const std::vector<std::string> test_strings = {
  "a",           "ab",
  "ab ",         " ab",
  " ab ",        " aa bb   cc dd  ee  ff  ",
  "\"a\"",       " \"a\" ",
  " \"aa \" bb", "aa bb \"cc\"",
  "aa bb \"cc\""
};
const std::vector<Tokens> answer_list = {
  { "a" },
  { "ab" },
  { "ab" },
  { "ab" },
  { "ab" },
  { "aa", "bb", "cc", "dd", "ee", "ff" },
  { "a" },
  { "a" },
  { "aa ", "bb" },
  { "aa", "bb", "cc" },
  { "aa", "bb", "cc" }
};

int
main()
{
  assert(test_strings.size() == answer_list.size());

  for (unsigned int i = 0; i < test_strings.size(); i++) {
    Tokens answer = split_to_tokens(test_strings[i]);
    assert(compare_tokens(answer, answer_list[i]));
  }
  return 0;
}

void new_game(IOSystem&, std::string)
{
  return;
}
