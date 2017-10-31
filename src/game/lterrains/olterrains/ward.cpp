#ifdef HEADER_PHASE
OLTERRAIN(ward, olterrain)
{
public:
  virtual truth IsWard () const;
};


#else


truth ward::IsWard () const { return true; }


#endif
