#ifdef HEADER_PHASE
ITEM(lantern, item)
{
 public:
  virtual void SignalSquarePositionChange(int);
  virtual truth AllowAlphaEverywhere() const { return true; }
  virtual int GetSpecialFlags() const;
  virtual truth IsLanternOnWall() const { return GetSquarePosition() != CENTER; }
 protected:
  virtual int GetClassAnimationFrames() const { return !IsBroken() ? 32 : 1; }
  virtual col16 GetMaterialColorA(int) const;
  virtual col16 GetMaterialColorB(int) const;
  virtual col16 GetMaterialColorC(int) const;
  virtual col16 GetMaterialColorD(int) const;
  virtual alpha GetAlphaA(int) const { return 255; }
  virtual alpha GetAlphaB(int) const;
  virtual alpha GetAlphaC(int) const;
  virtual alpha GetAlphaD(int) const;
  virtual v2 GetBitmapPos(int) const;
};


#else



col16 lantern::GetMaterialColorA(int) const { return MakeRGB16(255, 255, 240); }



col16 lantern::GetMaterialColorB(int) const { return MakeRGB16(255, 255, 100); }



col16 lantern::GetMaterialColorC(int) const { return MakeRGB16(255, 255, 100); }



col16 lantern::GetMaterialColorD(int) const { return MakeRGB16(255, 255, 100); }



void lantern::SignalSquarePositionChange(int SquarePosition)
{
  item::SignalSquarePositionChange(SquarePosition);
  UpdatePictures();
}



int lantern::GetSpecialFlags() const
{
  switch(GetSquarePosition())
  {
   case LEFT: return ROTATE|MIRROR;
   case DOWN: return FLIP;
   case UP: return 0;
   case RIGHT: return ROTATE;
  }

  return 0;
}



alpha lantern::GetAlphaB(int Frame) const
{
  Frame &= 31;
  return (Frame * (31 - Frame) >> 1);
}



alpha lantern::GetAlphaC(int Frame) const
{
  Frame &= 31;
  return (Frame * (31 - Frame) >> 2);
}



alpha lantern::GetAlphaD(int Frame) const
{
  Frame &= 31;
  return (Frame * (31 - Frame) >> 3);
}



v2 lantern::GetBitmapPos(int Frame) const
{
  return GetSquarePosition() == CENTER
            ? item::GetBitmapPos(Frame)
            : item::GetWallBitmapPos(Frame);
}
#endif
