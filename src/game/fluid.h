/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */
#ifndef __FLUID_H__
#define __FLUID_H__

#include "lsquare.h"
#include "trap.h"


class bitmap;


typedef truth (rawbitmap::*pixelpredicate) (v2) const;

class fluid : public entity {
public:
  /* Come To The Dark Side */
  fluid *Next;

public:
  fluid ();
  fluid (liquid *, lsquare *);
  fluid (liquid *, item *, cfestring &, truth);
  virtual ~fluid ();

  virtual void Be ();
  void Save (outputfile &) const;
  void Load (inputfile &);
  void SimpleDraw (blitdata &) const;
  void Draw (blitdata &) const;
  inline liquid *GetLiquid () const { return Liquid; }
  virtual square *GetSquareUnderEntity (int = 0) const { return LSquareUnder; }
  inline square *GetSquareUnder () const { return LSquareUnder; }
  inline void SetLSquareUnder (lsquare *What) { LSquareUnder = What; }
  inline lsquare *GetLSquareUnder () const { return LSquareUnder; }
  virtual truth IsOnGround () const { return true; }
  void AddLiquid (sLong);
  void AddLiquidAndVolume (sLong);
  virtual void SignalVolumeAndWeightChange ();
  void SetMotherItem (item *);
  static void AddFluidInfo (const fluid *, festring &);
  void CheckGearPicture (v2, int, truth);
  void DrawGearPicture (blitdata &, int) const;
  truth FadePictures ();
  void DrawBodyArmorPicture (blitdata &, int) const;
  void Redistribute ();
  virtual material *RemoveMaterial (material *);
  void Destroy ();
  inline cfestring &GetLocationName () const { return LocationName; }
  inline truth IsInside () const { return Flags & FLUID_INSIDE; }
  truth UseImage() const;
  virtual int GetTrapType () const { return Liquid->GetType() | FLUID_TRAP; }
  virtual uLong GetTrapID () const { return TrapData.TrapID; }
  virtual uLong GetVictimID () const { return TrapData.VictimID; }
  virtual void AddTrapName (festring &, int) const;
  virtual void UnStick () { TrapData.VictimID = 0; }
  virtual void UnStick (int I) { TrapData.BodyParts &= ~(1 << I); }
  void StepOnEffect (character *);
  virtual truth TryToUnStick (character *, v2);
  virtual void PreProcessForBone ();
  virtual void PostProcessForBone ();
  virtual truth IsStuckTo (ccharacter *) const;
  truth IsDangerous (ccharacter *) const;

protected:
  trapdata TrapData;
  struct imagedata {
    imagedata (truth = true);
    ~imagedata ();
    void Animate (blitdata &, int) const;
    void AddLiquidToPicture (const rawbitmap *, sLong, sLong, col16, pixelpredicate);
    void Save (outputfile &) const;
    void Load (inputfile &);
    truth Fade ();
    void Clear (truth);
    /* Only pictures of fluids not on ground have their RandMaps initialized,
       since they are animated. Note that the picture is always unrotated. */
    bitmap *Picture;
    /* Used by Animate() */
    mutable int DripTimer;
    mutable v2 DripPos;
    mutable col16 DripColor;
    mutable alpha DripAlpha;
    /* Sum of all alphas of Picture. The volume of the liquid is currently
       proportional to AlphaSum of the fluid's Image, limiting it
       considerably. */
    sLong AlphaSum;
    /* AlphaSum / (non-transparent pixels in Picture), used to synchronise
       gear pictures with the main image */
    packalpha AlphaAverage;
    /* The position of a gear picture in humanoid.pcx which binds the fluid;
       remembered so that it can be easily determined whether the fluid needs
       to be redistributed due to a major graphics change */
    v2 ShadowPos;
    /* Animation of gear items needs to know whether the raw picture is
       rotated somehow. Currently this is the case only for an item
       in the left hand of a character. */
    int SpecialFlags;
  };
  liquid *Liquid;
  lsquare *LSquareUnder;
  /* MotherItem == 0 means that the fluid is on ground */
  item *MotherItem;
  imagedata Image;
  /* Data of pictures shown over the player if he equips the item which
     the fluid covers. Note that these are not destroyed when the armor
     or weapon is unequipped. There is no real need, since the existence
     of the fluid is very temporary anyway. */
  imagedata *GearImage;
  uLong Flags;
  festring LocationName;
  static const sLong BodyArmorPartPixels[];
};


outputfile &operator << (outputfile &, const fluid *);
inputfile &operator >> (inputfile &, fluid *&);


#endif
