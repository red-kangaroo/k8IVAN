#ifdef HEADER_PHASE
GWTERRAIN(ocean, gwterrain)
{
public:
  ocean () { SetAnimationFrames(32); }
  virtual cchar *GetNameStem () const;
  virtual truth UsesLongArticle () const { return true; }
  virtual v2 GetBitmapPos (int) const;
  virtual int GetPriority () const { return 10; }
  virtual cchar *SurviveMessage () const;
  virtual cchar *MonsterSurviveMessage () const;
  virtual cchar *DeathMessage () const;
  virtual cchar *MonsterDeathVerb () const;
  virtual cchar *ScoreEntry () const;
  virtual truth IsFatalToStay () const { return true; }
  virtual int GetWalkability () const;
};


#else



cchar *ocean::GetNameStem () const { return "ocean"; }



v2 ocean::GetBitmapPos (int Frame) const { return v2(48 + ((Frame << 3)&~8), 48); }



cchar *ocean::SurviveMessage () const { return "you manage to reach the shore"; }



cchar *ocean::MonsterSurviveMessage () const { return "manages to reach the shore"; }



cchar *ocean::DeathMessage () const { return "you drown"; }



cchar *ocean::MonsterDeathVerb () const { return "drowns"; }



cchar *ocean::ScoreEntry () const { return "drowned"; }



int ocean::GetWalkability () const { return ANY_MOVE&~WALK; }
#endif
