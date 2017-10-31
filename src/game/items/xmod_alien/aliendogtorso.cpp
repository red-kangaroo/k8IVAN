#ifdef HEADER_PHASE
ITEM(aliendogtorso, normaltorso)
{
 protected:
  virtual int GetClassAnimationFrames () const { return 64; }
  virtual v2 GetBitmapPos (int) const;
};


#else


v2 aliendogtorso::GetBitmapPos (int Frame) const {
  v2 BasePos = torso::GetBitmapPos(Frame);
  switch (Frame) {
    case 1: case 14: case 15: case 16: return v2(BasePos.X+16, BasePos.Y);
    case 2: case 3: case 8: case 9: case 17: case 18: case 23: case 24: return v2(BasePos.X+32, BasePos.Y);
    case 4: case 5: case 10: case 11: case 19: case 20: case 25: case 26: return v2(BasePos.X+48, BasePos.Y);
    case 6: case 7: case 12:case 13: case 21: case 22: case 27: case 28: return v2(BasePos.X+64, BasePos.Y);
    case 0: case 29: return v2(BasePos.X+80, BasePos.Y);
    default: return v2(BasePos.X, BasePos.Y);
  }
}


#endif
