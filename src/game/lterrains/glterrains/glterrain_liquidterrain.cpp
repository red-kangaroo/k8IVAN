#ifdef HEADER_PHASE
GLTERRAIN(liquidterrain, glterrain)
{
public:
  virtual cchar *SurviveMessage () const;
  virtual cchar *MonsterSurviveMessage () const;
  virtual cchar *DeathMessage () const;
  virtual cchar *MonsterDeathVerb () const;
  virtual cchar *ScoreEntry () const;
  virtual truth IsFatalToStay () const;
  virtual truth DipInto (item *, character *);
  virtual truth IsDipDestination () const;
  virtual void SurviveEffect (character *);
  virtual void AddLocationDescription (festring &) const;

protected:
  virtual void AddPostFix (festring &String, int) const;
  virtual int GetClassAnimationFrames () const;
  virtual v2 GetBitmapPos (int) const;
};


#else


truth liquidterrain::IsFatalToStay () const { return true; }
truth liquidterrain::IsDipDestination () const { return true; }
void liquidterrain::AddPostFix (festring & String, int) const { AddLumpyPostFix(String); }
int liquidterrain::GetClassAnimationFrames () const { return 32; }
cchar *liquidterrain::SurviveMessage () const { return "you manage to get out of the pool"; }
cchar *liquidterrain::MonsterSurviveMessage () const { return "manages to get out of the pool"; }
cchar *liquidterrain::DeathMessage () const { return "you drown"; }
cchar *liquidterrain::MonsterDeathVerb () const { return "drowns"; }
cchar *liquidterrain::ScoreEntry () const { return "drowned"; }


truth liquidterrain::DipInto (item *ToBeDipped, character* Who) {
  ToBeDipped->DipInto(static_cast<liquid*>(GetMainMaterial()->SpawnMore(100)), Who);
  return true;
}


void liquidterrain::SurviveEffect (character *Survivor) {
  Survivor->GetLSquareUnder()->SpillFluid(Survivor, static_cast<liquid*>(GetMainMaterial()->SpawnMore(1000 + RAND_N(500))), false, false);
}


v2 liquidterrain::GetBitmapPos (int F) const {
  /* gum solution, should come from script */
  return GetBorderBitmapPos(v2(64 + (F >> 1 & 3) * 48, 32 + (F >> 3 & 3) * 48), F);
}


void liquidterrain::AddLocationDescription (festring &String) const {
  String << " in the " << GetNameSingular();
}


#endif
