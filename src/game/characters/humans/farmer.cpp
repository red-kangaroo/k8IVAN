#ifdef HEADER_PHASE
CHARACTER(farmer, humanoid)
{
 public:
  virtual v2 GetHeadBitmapPos() const;
  virtual v2 GetRightArmBitmapPos() const;
  virtual v2 GetLeftArmBitmapPos() const { return GetRightArmBitmapPos(); }
};


#else



v2 farmer::GetHeadBitmapPos() const { return v2(96, (4 + (RAND() & 1)) << 4); }



v2 farmer::GetRightArmBitmapPos() const { return v2(64, (RAND() & 1) << 4); }
#endif
