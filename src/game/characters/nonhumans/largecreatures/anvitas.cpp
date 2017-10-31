#ifdef HEADER_PHASE
CHARACTER(anvitas, largecreature)
{
public:
  virtual void SpecialBodyDefenceEffect (character *, bodypart *, int);
};


#else


void anvitas::SpecialBodyDefenceEffect (character *Enemy, bodypart *BodyPart, int Type) {
  if (Type != WEAPON_ATTACK && (RAND()&1)) {
    if (Enemy->IsPlayer()) ADD_MESSAGE("%s spines jab your %s!", CHAR_POSSESSIVE_PRONOUN, BodyPart->GetBodyPartName().CStr());
    else if (CanBeSeenByPlayer() || Enemy->CanBeSeenByPlayer()) ADD_MESSAGE("%s spines jab %s!", CHAR_POSSESSIVE_PRONOUN, Enemy->CHAR_NAME(DEFINITE));
    Enemy->ReceiveBodyPartDamage(this, 10+(RAND()&6), PHYSICAL_DAMAGE, BodyPart->GetBodyPartIndex(), YOURSELF, false, false, true, false);
    Enemy->CheckDeath(CONST_S("killed by the pointy spines of ") + GetName(INDEFINITE), this);
  }
}


#endif
