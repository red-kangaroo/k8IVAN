#ifdef HEADER_PHASE
CHARACTER(frog, nonhumanoid)
{
 public:
  virtual truth MoveRandomly() { return MoveRandomlyInRoom(); }
};
#endif
