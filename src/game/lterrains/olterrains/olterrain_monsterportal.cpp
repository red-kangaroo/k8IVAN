#ifdef HEADER_PHASE
OLTERRAIN(monsterportal, olterrain)
{
 public:
  monsterportal();
 protected:
  virtual int GetClassAnimationFrames() const { return 32; }
  virtual v2 GetBitmapPos(int) const;
};


#else



v2 monsterportal::GetBitmapPos (int Frame) const { return v2(16 + (((Frame & 31) << 3)&~8), 0); } // gum solution, should come from script



monsterportal::monsterportal () {
  game::SetMonsterPortal(this);
}
#endif
