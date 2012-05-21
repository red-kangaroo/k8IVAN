#ifdef HEADER_PHASE
CHARACTER(haastseagle, largecreature)
{
protected:
  virtual cchar *FirstPersonBiteVerb () const;
  virtual cchar *FirstPersonCriticalBiteVerb () const;
  virtual cchar *ThirdPersonBiteVerb () const;
  virtual cchar *ThirdPersonCriticalBiteVerb () const;
};


#else


cchar *haastseagle::FirstPersonBiteVerb() const { return "peck"; }
cchar *haastseagle::FirstPersonCriticalBiteVerb() const { return "critically peck"; }
cchar *haastseagle::ThirdPersonBiteVerb() const { return "pecks"; }
cchar *haastseagle::ThirdPersonCriticalBiteVerb() const { return "critically pecks"; }


#endif
