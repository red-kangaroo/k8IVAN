#ifdef HEADER_PHASE
ITEM(adultalientorso, normaltorso)
{
 protected:
  virtual int GetClassAnimationFrames () const { return 64; }
  virtual v2 GetBitmapPos (int) const;
};


#else


v2 adultalientorso::GetBitmapPos (int Frame) const {
  v2 BasePos = torso::GetBitmapPos(Frame);
  switch (Frame) {
    case 0: case 1: case 2: case 3: case 10: case 11: case 12: case 13: return v2(BasePos.X+16, BasePos.Y);
    case 4: case 5: case 6: case 7: case 8: case 9: return v2(BasePos.X+32, BasePos.Y);
    default: return v2(BasePos.X, BasePos.Y);
  }
}


#endif
