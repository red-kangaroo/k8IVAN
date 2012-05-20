#ifdef HEADER_PHASE
OLTERRAIN(sign, olterrain)
{
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void SetText(cfestring& What) { Text = What; }
  virtual festring GetText() const;
  virtual void AddPostFix(festring&, int) const;
  virtual void StepOn(character*);
 protected:
  festring Text;
};


#else



festring sign::GetText () const { return Text; }



void sign::AddPostFix (festring &String, int) const {
  String << " with text \"" << Text << '\"';
}



void sign::StepOn (character *Stepper) {
  if (Stepper->IsPlayer()) ADD_MESSAGE("There's a sign here saying: \"%s\"", Text.CStr());
}



void sign::Save (outputfile &SaveFile) const {
  olterrain::Save(SaveFile);
  SaveFile << Text;
}



void sign::Load (inputfile &SaveFile) {
  olterrain::Load(SaveFile);
  SaveFile >> Text;
}
#endif
