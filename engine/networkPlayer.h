class networkPlayer : public player{
 public:
  ~networkPlayer();
  networkPlayer();
  void      *input_buffer;
  int       input_buffer_size;
  unsigned int state = DISCONNECTED;
  void       ping();
  bool synced = false;
};
