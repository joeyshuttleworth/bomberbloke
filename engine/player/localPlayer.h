class localPlayer : public player{
 public:
  std::list<command_binding> mControlScheme;
  localPlayer(std::string);
};
