#ifdef HEADER_PHASE
ITEM(dogtorso, normaltorso)
{
 protected:
  virtual void Draw(blitdata&) const;
  virtual int GetClassAnimationFrames() const { return 16; }
  virtual v2 GetBitmapPos(int) const;
};


#else



v2 dogtorso::GetBitmapPos(int Frame) const
{
  v2 BasePos = torso::GetBitmapPos(Frame);

  if(Frame >= GraphicData.AnimationFrames >> 1)
    BasePos.X += 32;

  return v2(BasePos.X + ((Frame & 4) << 2), BasePos.Y);
}



void dogtorso::Draw(blitdata& BlitData) const
{
  cint AF = GraphicData.AnimationFrames >> 1;
  int Index = !(BlitData.CustomData & ALLOW_ANIMATE) || AF == 1 ? 0 : GET_TICK() & (AF - 1);

  if(GetHP() << 1 <= GetMaxHP())
    Index += AF;

  cbitmap* P = GraphicData.Picture[Index];

  if(BlitData.CustomData & ALLOW_ALPHA)
    P->AlphaPriorityBlit(BlitData);
  else
    P->MaskedPriorityBlit(BlitData);
}
#endif
