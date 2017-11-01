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


#endif
