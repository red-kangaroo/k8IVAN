#ifdef HEADER_PHASE
ACTION(go, action)
{
public:
  go () : Direction(0), WalkingInOpen(false), mPrevWasTurn(false) {}

  virtual void Save (outputfile &SaveFile) const;
  virtual void Load (inputfile &SaveFile);
  virtual void Handle ();
  int GetDirection () const { return Direction; }
  void SetDirection (int What) { Direction = What; }
  truth IsWalkingInOpen () const { return WalkingInOpen; }
  void SetIsWalkingInOpen (truth What) { WalkingInOpen = What; }
  virtual truth TryDisplace ();
  virtual cchar *GetDescription () const;
  virtual truth ShowEnvironment () const { return false; }

  inline truth prevWasTurn () const { return mPrevWasTurn; }
  inline void SetPrevWasTurn (truth v) { mPrevWasTurn = v; }

protected:
  int Direction;
  truth WalkingInOpen;
  truth mPrevWasTurn;
};


#else


cchar *go::GetDescription () const { return "going"; }


void go::Save (outputfile &SaveFile) const {
  action::Save(SaveFile);
  SaveFile << Direction << WalkingInOpen;
}


void go::Load (inputfile &SaveFile) {
  action::Load(SaveFile);
  SaveFile >> Direction >> WalkingInOpen;
}


void go::Handle () {
  GetActor()->EditAP(GetActor()->GetStateAPGain(100)); // gum solution
  GetActor()->GoOn(this);
}


truth go::TryDisplace () {
  Terminate(false);
  return true;
}


#endif
