#ifdef HEADER_PHASE
CHARACTER(werewolfwolf, humanoid)
{
 public:
  virtual festring GetKillName() const;
};


#else



festring werewolfwolf::GetKillName() const
{
  if(GetPolymorphBackup() && GetPolymorphBackup()->GetType() == werewolfhuman::ProtoType.GetIndex())
    return GetName(INDEFINITE);

  return humanoid::GetKillName();
}
#endif
