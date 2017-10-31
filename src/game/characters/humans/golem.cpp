#ifdef HEADER_PHASE
CHARACTER(golem, humanoid)
{
 public:
  golem();
  virtual truth MoveRandomly();
  virtual truth CheckForUsefulItemsOnGround(truth = true) { return false; }
  virtual void BeTalkedTo();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  void SetItemVolume(sLong What) { ItemVolume = What; }
 protected:
  virtual truth AddAdjective(festring&, truth) const;
  virtual material* CreateBodyPartMaterial(int, sLong) const;
  virtual void CreateCorpse(lsquare*);
  sLong ItemVolume;
};


#else



material* golem::CreateBodyPartMaterial(int, sLong Volume) const { return MAKE_MATERIAL(GetConfig(), Volume); }



truth golem::MoveRandomly()
{
  if(!(RAND() % 500))
  {
    Engrave(CONST_S("Golem Needs Master"));
    EditAP(-1000);
    return true;
  }
  else
    return character::MoveRandomly();
}



void golem::BeTalkedTo()
{
  static sLong Said;

  if(GetRelation(PLAYER) == HOSTILE)
    Engrave(GetHostileReplies()[RandomizeReply(Said, GetHostileReplies().Size)]);
  else
    Engrave(GetFriendlyReplies()[RandomizeReply(Said, GetFriendlyReplies().Size)]);

  if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s engraves something.", CHAR_NAME(DEFINITE));
}



truth golem::AddAdjective(festring& String, truth Articled) const
{
  int TotalRustLevel = sumbodypartproperties()(this, &bodypart::GetMainMaterialRustLevel);

  if(!TotalRustLevel)
    return humanoid::AddAdjective(String, Articled);
  else
  {
    if(Articled)
      String << "a ";

    if(TotalRustLevel <= GetBodyParts())
      String << "slightly rusted ";
    else if(TotalRustLevel <= GetBodyParts() << 1)
      String << "rusted ";
    else
      String << "very rusted ";

    String << GetAdjective() << ' ';
    return true;
  }
}



void golem::CreateCorpse(lsquare* Square)
{
  material* Material = GetTorso()->GetMainMaterial();

  if(Material->IsSolid())
    Square->AddItem(Material->CreateNaturalForm(ItemVolume));

  SendToHell();
}



golem::golem()
{
  if(!game::IsLoading())
    ItemVolume = 50 + RAND_N(100);
}



void golem::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << ItemVolume;
}



void golem::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> ItemVolume;
}
#endif
