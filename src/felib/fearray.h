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
#ifndef __FELIB_FEARRAY_H__
#define __FELIB_FEARRAY_H__

#include "femath.h"


template <class type> struct fearray {
public:
  typedef uInt sizetype;

public:
  fearray () : Data(0), Size(0) {}
  fearray (const fearray &arr);
  fearray (const type *arr, sizetype aSize);
  ~fearray ();

  fearray &operator = (const fearray &arr);
  type &operator [] (sizetype idx) { return Data[idx]; }
  const type &operator [] (sizetype idx) const { return Data[idx]; }

  void Allocate (sizetype count);
  void Add (const type &it);
  void Clear ();

  const type &GetRandomElement () const { return Data[RAND_N(Size)]; }

public:
  type *Data;
  sizetype Size;
};


template <class type> inline fearray<type>::fearray (const fearray<type>& arr) : Data(arr.Data), Size(arr.Size) {
  if (Data) ++REFS(Data);
}


template <class type> inline fearray<type>::fearray (const type *arr, sizetype aSize) : Data(0), Size(aSize) {
  char *ptr = new char[aSize*sizeof(type)+sizeof(rcint)];
  *reinterpret_cast<rcint *>(ptr) = 0;
  Data = reinterpret_cast<type *>(ptr+sizeof(rcint));
  for (sizetype c = 0; c < aSize; ++c) new (&Data[c])type(arr[c]);
}


template <class type> inline fearray<type>::~fearray<type> () {
  Clear();
}


template <class type> inline fearray<type> &fearray<type>::operator = (const fearray<type> &arr) {
  if (arr.Data != Data) Clear();
  Data = arr.Data;
  if (Data) ++REFS(Data);
  Size = arr.Size;
  return *this;
}


template <class type> inline void fearray<type>::Clear () {
  type *ptr = Data;
  if (ptr) {
    if (!REFS(ptr)--) {
      for (sizetype c = 0; c < Size; ++c) ptr[c].~type();
      delete [] REFSA(ptr);
    }
    Data = 0;
    Size = 0;
  }
}


template <class type> inline void fearray<type>::Allocate (sizetype count) {
  char *ptr = new char[count*sizeof(type)+sizeof(rcint)];
  *reinterpret_cast<rcint *>(ptr) = 0;
  Data = reinterpret_cast<type *>(ptr+sizeof(rcint));
  Size = count;
  for (sizetype c = 0; c < count; ++c) new (&Data[c])type;
}


/* Don't use unless necessary */
template <class type> inline void fearray<type>::Add (const type &it) {
  type *ptr = Data;
  if (ptr) {
    sizetype oldsize = this->Size++;
    char *newptr = new char[Size*sizeof(type)+sizeof(rcint)];
    *reinterpret_cast<rcint *>(newptr) = 0;
    type *newdata = reinterpret_cast<type *>(newptr+sizeof(rcint));
    if (!REFS(ptr)--) {
      for (sizetype c = 0; c < oldsize; ++c) {
        new (&newdata[c])type(ptr[c]);
        ptr[c].~type();
      }
      delete [] REFSA(ptr);
    } else {
      for(sizetype c = 0; c < oldsize; ++c) new (&newdata[c])type(ptr[c]);
    }
    Data = newdata;
    new(&newdata[oldsize]) type(it);
  } else {
    char *newptr = new char[sizeof(type)+sizeof(rcint)];
    *reinterpret_cast<int*>(newptr) = 0;
    Data = reinterpret_cast<type*>(newptr+sizeof(rcint));
    Size = 1;
    new(Data) type(it);
  }
}


template <class type1, class type2> inline void ArrayToVector (const fearray<type1> &arr, std::vector<type2> &vect) {
  vect.resize(arr.Size, type2());
  for (typename fearray<type1>::sizetype c = 0; c < arr.Size; ++c) vect[c] = arr.Data[c];
}


#endif
