#ifdef HEADER_PHASE
ITEM(largecorpse, corpse)
{
 public:
  virtual void SignalStackAdd(stackslot*, void (stack::*)(item*, truth));
  virtual int GetSquareIndex(v2) const;
  virtual void Draw(blitdata&) const;
  virtual void CalculateSquaresUnder() { SquaresUnder = 4; }
 protected:
  virtual v2 GetBitmapPos(int I) const { return GetLargeBitmapPos(item::GetBitmapPos(I), I); }
  virtual void ModifyAnimationFrames(int& AF) const { AF <<= 2; }
};


#else



void largecorpse::Draw(blitdata& BlitData) const
{
  LargeDraw(BlitData);
}



void largecorpse::SignalStackAdd(stackslot* StackSlot, void (stack::*AddHandler)(item*, truth))
{
  if(!Slot[0])
  {
    Slot[0] = StackSlot;
    v2 Pos = GetPos();
    level* Level = GetLevel();

    for(int c = 1; c < 4; ++c)
      (Level->GetLSquare(Pos + game::GetLargeMoveVector(12 + c))->GetStack()->*AddHandler)(this, false);
  }
  else
    for(int c = 1; c < 4; ++c)
      if(!Slot[c])
      {
  Slot[c] = StackSlot;
  return;
      }
}



int largecorpse::GetSquareIndex(v2 Pos) const
{
  v2 RelativePos = Pos - GetPos();
  return RelativePos.X + (RelativePos.Y << 1);
}
#endif
