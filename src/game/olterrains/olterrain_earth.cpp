#ifdef HEADER_PHASE
OLTERRAIN(earth, olterrain)
{
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
 protected:
  virtual void PostConstruct();
  virtual v2 GetBitmapPos(int) const;
  int PictureIndex;
};


#else



void earth::PostConstruct () {
  PictureIndex = RAND() & 3;
}



void earth::Save (outputfile &SaveFile) const {
  olterrain::Save(SaveFile);
  SaveFile << PictureIndex;
}



void earth::Load (inputfile& SaveFile) {
  olterrain::Load(SaveFile);
  SaveFile >> PictureIndex;
}



v2 earth::GetBitmapPos (int I) const {
  return olterrain::GetBitmapPos(I) + v2(PictureIndex * 48, 0);
}
#endif
