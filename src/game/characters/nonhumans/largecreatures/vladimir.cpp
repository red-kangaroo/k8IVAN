#ifdef HEADER_PHASE
CHARACTER(vladimir, largecreature)
{
 public:
  //virtual truth MustBeRemovedFromBone() const;
  virtual col16 GetSkinColor() const;
  virtual void SpecialTurnHandler() { UpdatePictures(); }
};


#else



col16 vladimir::GetSkinColor() const { return MakeRGB16(60 + RAND() % 190, 60 + RAND() % 190, 60 + RAND() % 190); }


/*
truth vladimir::MustBeRemovedFromBone() const
{
  //return !IsEnabled() || GetTeam()->GetID() != IVAN_TEAM || GetDungeon()->GetIndex() != ELPURI_CAVE|| GetLevel()->GetIndex() != IVAN_LEVEL;
  return (!IsEnabled() || GetTeam()->GetID() != IVAN_TEAM || IsHomeLevel(GetLevel()));
}
*/


#endif
