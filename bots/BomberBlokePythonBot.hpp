#ifndef PYTHONBOT_HPP
#define PYTHONBOT_HPP


class __attribute__((visibility("hidden"))) BomberBlokePythonBot : public PythonBot{
protected:
public:
  virtual ~BomberBlokePythonBot();
  using PythonBot::PythonBot

  std::set<std::array<int, 2>> getDangerousSquares(int timeout = 0);

}
#endif
