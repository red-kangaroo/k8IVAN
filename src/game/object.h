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
#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "ivancommon.h"

#include "igraph.h"
#include "entity.h"
#include "id.h"
#include "fearray.h"
#include "festring.h"


class god;
class object;


typedef v2 (object::*bposretriever) (int) const;


class object : public entity, public id {
public:
  object ();
  object (const object &);
  virtual ~object ();

  virtual void Save (outputfile &) const;
  virtual void Load (inputfile &);
  virtual void UpdatePictures ();
  material *GetMainMaterial () const { return MainMaterial; }
  virtual material *GetSecondaryMaterial () const { return 0; }
  virtual void SetSecondaryMaterial (material *, int = 0) { }
  virtual void ChangeSecondaryMaterial (material *, int = 0) { }
  virtual int GetMaterials () const { return 1; }
  virtual material *GetMaterial (int) const { return MainMaterial; }
  cbitmap* const *GetPicture () const;
  virtual col24 GetBaseEmitation () const { return 0; }
  virtual void SetParameters (int) { }
  virtual int GetOKVisualEffects () const { return 0; }
  int GetVisualEffects () const { return VisualEffects; }
  void SetVisualEffects (int What) { VisualEffects = What; }
  virtual int GetForcedVisualEffects () const { return 0; }
  int GetAnimationFrames () const { return GraphicData.AnimationFrames; }
  virtual truth IsAnimated () const { return GraphicData.AnimationFrames > 1; }
  virtual void CalculateEmitation ();
  void LoadMaterial (inputfile &, material *&);
  virtual const fearray<sLong> &GetMaterialConfigChances () const = 0;
  virtual sLong GetMaterialConfigChanceSum () const = 0;
  virtual void CalculateAll () = 0;
  virtual int GetSpoilLevel () const { return 0; }
  void CreateWieldedBitmap (graphicid &) const;
  virtual int GetSpecialFlags () const;
  static void InitSparkleValidityArrays ();
  void UpdatePictures (graphicdata &, v2, int, alpha, int, bposretriever) const;
  void InitMaterial (material *&, material *, sLong);
  virtual truth DetectMaterial (cmaterial *) const;
  virtual int GetSparkleFlags () const;
  virtual void SignalMaterialChange () {}

public:
  festring mOnEvents;

protected:
  void CopyMaterial (material *const &, material *&);
  void ObjectInitMaterials (material *&, material *, sLong, material *&, material *, sLong, truth);
  material *SetMaterial (material *&, material *, sLong, int);
  void ChangeMaterial (material *&, material *, sLong, int);
  virtual truth CalculateHasBe () const;
  virtual int GetGraphicsContainerIndex () const = 0;
  virtual col16 GetMaterialColorA (int) const;
  virtual col16 GetMaterialColorB (int) const { return 0; }
  virtual col16 GetMaterialColorC (int) const { return 0; }
  virtual col16 GetMaterialColorD (int) const { return 0; }
  virtual alpha GetMaxAlpha () const { return 255; }
  virtual alpha GetAlphaA (int) const;
  virtual alpha GetAlphaB (int) const { return 255; }
  virtual alpha GetAlphaC (int) const { return 255; }
  virtual alpha GetAlphaD (int) const { return 255; }
  virtual col16 GetOutlineColor (int) const;
  virtual alpha GetOutlineAlpha (int) const { return 255; }
  virtual truth AddRustLevelDescription (festring &, truth) const;
  virtual truth AddMaterialDescription( festring &, truth) const;
  int RandomizeMaterialConfiguration ();
  virtual int GetClassAnimationFrames () const { return 1; }
  void AddContainerPostFix (festring &) const;
  void AddLumpyPostFix (festring &) const;
  truth AddEmptyAdjective (festring &, truth) const;
  virtual v2 GetBitmapPos (int) const = 0;
  void RandomizeVisualEffects ();
  virtual void ModifyAnimationFrames (int &) const { }
  virtual int GetRustDataA () const;
  virtual int GetRustDataB () const { return NOT_RUSTED; }
  virtual int GetRustDataC () const { return NOT_RUSTED; }
  virtual int GetRustDataD () const { return NOT_RUSTED; }
  virtual col16 GetDripColor () const { return 0; }
  virtual truth AllowSparkling () const { return true; }
  virtual truth AllowRegularColors () const { return true; }
  virtual int GetWobbleData () const { return 0; }
  truth RandomizeSparklePos (v2 &, v2, int &, feuLong, int, int) const;

//public:
  //virtual truth IsABone () const { return false; }
  //virtual truth IsASkull () const { return false; }
  virtual truth IsBoneNameSingular () const { return false; }

protected:
  festring mDefFile;
  graphicdata GraphicData;
  material *MainMaterial;
  int VisualEffects;
};


#endif
