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
#include <cmath>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "femath.h"
#include "feerror.h"
#include "fesave.h"


cint basequadricontroller::OrigoDeltaX[4] = { 0, 1, 0, 1 };
cint basequadricontroller::OrigoDeltaY[4] = { 0, 0, 1, 1 };
int basequadricontroller::OrigoX, basequadricontroller::OrigoY;
int basequadricontroller::StartX, basequadricontroller::StartY;
int basequadricontroller::XSize, basequadricontroller::YSize;
int basequadricontroller::RadiusSquare;
truth basequadricontroller::SectorCompletelyClear;
PCG32 femath::prng = PCG32(0x29a, 42);
PRNGSeed femath::savedSeed;


// ////////////////////////////////////////////////////////////////////////// //
void PCG32::rndseed () {
  seed.state = time(NULL);
  seed.inc = 1;
  int fd = open("/dev/urandom", O_RDONLY);
  if (fd < 0) return;
  read(fd, &seed.state, sizeof(seed.state));
  //read(fd, &inc, sizeof(inc));
  close(fd);
}


// ////////////////////////////////////////////////////////////////////////// //
void femath::SetSeed (const PRNGSeed aseed) {
  prng.setSeed(aseed);
}


void femath::SetSeed (feuLong aseed) {
  prng.setSeed(aseed);
}


void femath::RandSeed () {
  prng.rndseed();
}


void femath::SaveSeed () {
  savedSeed = prng.getSeed();
}


void femath::LoadSeed () {
  prng.setSeed(savedSeed);
}


sLong femath::Rand () {
  return (prng.rand32()&0x7FFFFFFF);
}


int femath::WeightedRand (sLong *Possibility, sLong TotalPossibility) {
  sLong Rand = RAND()%TotalPossibility, PartialSum = 0;
  for (int c = 0; ; ++c) {
    PartialSum += Possibility[c];
    if (PartialSum > Rand) return c;
  }
}


int femath::WeightedRand (const std::vector<sLong> &Possibility, sLong TotalPossibility) {
  sLong Rand = RAND()%TotalPossibility, PartialSum = 0;
  for (int c = 0;; ++c) {
    PartialSum += Possibility[c];
    if (PartialSum > Rand) return c;
  }
}


double femath::CalculateAngle (v2 Direction) {
  if (Direction.X < 0) return atan(double(Direction.Y)/Direction.X)+FPI;
  if (Direction.X > 0) {
    if (Direction.Y < 0) return atan(double(Direction.Y)/Direction.X)+2*FPI;
    return atan(double(Direction.Y)/Direction.X);
  }
  if (Direction.Y < 0) return 3*FPI/2;
  if (Direction.Y > 0) return FPI/2;
  ABORT("Illegal direction (0, 0) passed to femath::CalculateAngle()!");
  return 0;
}


void femath::CalculateEnvironmentRectangle (rect &Rect, const rect &MotherRect, v2 Origo, int Radius) {
  Rect.X1 = Origo.X-Radius;
  Rect.Y1 = Origo.Y-Radius;
  Rect.X2 = Origo.X+Radius;
  Rect.Y2 = Origo.Y+Radius;
  if (Rect.X1 < MotherRect.X1) Rect.X1 = MotherRect.X1;
  if (Rect.Y1 < MotherRect.Y1) Rect.Y1 = MotherRect.Y1;
  if (Rect.X2 > MotherRect.X2) Rect.X2 = MotherRect.X2;
  if (Rect.Y2 > MotherRect.Y2) Rect.Y2 = MotherRect.Y2;
}


truth femath::Clip (int &SourceX, int &SourceY, int &DestX, int &DestY, int &Width, int &Height, int XSize, int YSize, int DestXSize, int DestYSize) {
  /* This sentence is usually true */
  if (SourceX >= 0 && SourceY >= 0 && DestX >= 0 && DestY >= 0 &&
      SourceX+Width <= XSize && SourceY+Height <= YSize &&
      DestX+Width <= DestXSize && DestY+Height <= DestYSize) return true;
  if (SourceX < 0) {
    Width += SourceX;
    DestX -= SourceX;
    SourceX = 0;
  }
  if (SourceY < 0) {
    Height += SourceY;
    DestY -= SourceY;
    SourceY = 0;
  }
  if (DestX < 0) {
    Width += DestX;
    SourceX -= DestX;
    DestX = 0;
  }
  if (DestY < 0) {
    Height += DestY;
    SourceY -= DestY;
    DestY = 0;
  }
  if (SourceX+Width > XSize) Width = XSize-SourceX;
  if (SourceY+Height > YSize) Height = YSize-SourceY;
  if (DestX+Width > DestXSize) Width = DestXSize-DestX;
  if (DestY+Height > DestYSize) Height = DestYSize-DestY;
  return Width>0 && Height>0;
}


void ReadData (interval &I, inputfile &SaveFile) {
  I.Min = SaveFile.ReadNumber(HIGHEST, true);
  festring Word;
  SaveFile.ReadWord(Word);
  if (Word == ";" || Word == ",") I.Max = I.Min;
  else if (Word == ":") I.Max = Max(SaveFile.ReadNumber(), I.Min);
  else ABORT("Odd interval terminator %s detected, file %s line %d!", Word.CStr(), SaveFile.GetFileName().CStr(), SaveFile.TokenLine());
}


void ReadData (region &R, inputfile &SaveFile) {
  ReadData(R.X, SaveFile);
  ReadData(R.Y, SaveFile);
}


outputfile &operator << (outputfile &SaveFile, const interval &I) {
  SaveFile.Write(reinterpret_cast<cchar *>(&I), sizeof(I));
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, interval &I) {
  SaveFile.Read(reinterpret_cast<char *>(&I), sizeof(I));
  return SaveFile;
}


outputfile &operator << (outputfile &SaveFile, const region &R) {
  SaveFile.Write(reinterpret_cast<cchar *>(&R), sizeof(R));
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, region &R) {
  SaveFile.Read(reinterpret_cast<char *>(&R), sizeof(R));
  return SaveFile;
}


outputfile &operator << (outputfile &SaveFile, const PRNGSeed &R) {
  SaveFile.Write((cchar *)R.seed, sizeof(R.seed));
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, PRNGSeed &R) {
  SaveFile.Read((char *)R.seed, sizeof(R.seed));
  return SaveFile;
}


void femath::SavePRNG (outputfile &SaveFile) {
  PRNGSeed seed = prng.getSeed();
  SaveFile << seed;
}


void femath::LoadPRNG (inputfile &SaveFile) {
  PRNGSeed seed;
  SaveFile >> seed;
  prng.setSeed(seed);
}


sLong femath::SumArray (const fearray<sLong> &Vector) {
  sLong Sum = 0;
  for (uInt c = 0; c < Vector.Size; ++c) Sum += Vector.Data[c];
  return Sum;
}


void femath::GenerateFractalMap (int **Map, int Side, int StartStep, int Randomness) {
  cint Limit = Side-1;
  Map[0][0] = 0;
  Map[0][Limit] = 0;
  Map[Limit][0] = 0;
  Map[Limit][Limit] = 0;
  for (int Step = StartStep, HalfStep = Step>>1; HalfStep;
    Step = HalfStep, HalfStep>>=1, Randomness = ((Randomness<<3)-Randomness)>>3)
  {
    int x, y, RandMod = (Randomness<<1)+1;

    for (x = HalfStep; x < Side; x += Step)
      for (y = HalfStep; y < Side; y += Step)
        Map[x][y] =
          ((Map[x-HalfStep][y-HalfStep]+
            Map[x-HalfStep][y+HalfStep]+
            Map[x+HalfStep][y-HalfStep]+
            Map[x+HalfStep][y+HalfStep])>>2)-Randomness+RAND()%RandMod;

    for (x = HalfStep; x < Side; x += Step) {
      for (y = 0; y < Side; y += Step) {
        int HeightSum = Map[x-HalfStep][y]+Map[x+HalfStep][y];
        int Neighbours = 2;
        if (y) {
          HeightSum += Map[x][y-HalfStep];
          ++Neighbours;
        }
        if (y != Limit) {
          HeightSum += Map[x][y+HalfStep];
          ++Neighbours;
        }
        if (Neighbours == 4) HeightSum >>= 2; else HeightSum /= Neighbours;
        Map[x][y] = HeightSum-Randomness+RAND()%RandMod;
      }
    }

    for (x = 0; x < Side; x += Step) {
      for (y = HalfStep; y < Side; y += Step) {
        int HeightSum = Map[x][y-HalfStep]+Map[x][y+HalfStep];
        int Neighbours = 2;
        if (x) {
          HeightSum += Map[x-HalfStep][y];
          ++Neighbours;
        }
        if (x != Limit) {
          HeightSum += Map[x+HalfStep][y];
          ++Neighbours;
        }
        if (Neighbours == 4) HeightSum >>= 2; else HeightSum /= Neighbours;
        Map[x][y] = HeightSum-Randomness+RAND()%RandMod;
      }
    }
  }
}
