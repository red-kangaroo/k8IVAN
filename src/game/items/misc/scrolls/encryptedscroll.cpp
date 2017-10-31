#ifdef HEADER_PHASE
ITEM(encryptedscroll, scroll)
{
 public:
  virtual void Be() { }
  virtual truth Read(character*);
  virtual truth ReceiveDamage(character*, int, int, int) { return false; }
  virtual truth IsEncryptedScroll() const { return true; }
};


#else



truth encryptedscroll::Read(character*)
{
  ADD_MESSAGE("You could never hope to decipher this top secret message. It is meant for Petrus's eyes only.");
  return false;
}
#endif
