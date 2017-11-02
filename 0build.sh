#!/bin/sh
# WARNING! SHITTY CODE!


defines="-DWIZARD -DLOCAL_SAVES"
cflags="-Wno-narrowing -Isrc/felib -m32"
lflags="-lm"
objects=""
have_package=""
BUILD_DIR="_build"
shitdoze="ona"
FORCE="ona"
linkflags=""

while [ $# -gt 0 ]; do
  if [ "z$1" = "zshitdoze" ]; then
    shitdoze="tan"
  elif [ "z$1" = "zforce" ]; then
    FORCE="tan"
  else
    echo "FATAL: unknown arg: $1"
    exit 1
  fi
  shift
done


if [ $shitdoze = tan ]; then
  ARCH="i686-w64-mingw32.static-"
  BINSUFF=".exe"
  defines="${defines} -DSHITDOZE"
  BUILD_DIR="${BUILD_DIR}_shitdoze"
  linkflags="-Wl,-subsystem,windows"
else
  ARCH=""
  BINSUFF=""
  cflags="${cflags} -Wno-misleading-indentation"
fi

GCC="${ARCH}gcc"
GPP="${ARCH}g++"
PKGCONFIG="${ARCH}pkg-config"


find_package() {
  if [ "z$2" != "z" ]; then
    $PKGCONFIG --silence-errors $1 --atleast-version=$2
  else
    $PKGCONFIG --silence-errors $1
  fi
  if [ "$?" = "0" ]; then
    cflags="${cflags} `$PKGCONFIG $1 --cflags`"
    lflags="${lflags} `$PKGCONFIG $1 --libs`"
    #echo "MSG: package '$1' found"
    have_package="tan"
  else
    #echo "MSG: no package '$1'"
    have_package="ona"
  fi
}


# $1: srcdir
# $2: srcfile
# $3: use gcc if not empty
compile() {
  local obj
  local xname
  local gppopt
  local fstime
  local fotime
  #
  obj=`basename $2 .cpp`
  obj=`basename $obj .c`
  obj="${obj}.o"
  obj="${BUILD_DIR}/${obj}"
  objects="${objects} ${obj}"
  xname=`basename -s .c "$2"`
  if [ "z$xname" = "z$2" ]; then
    echo "C++     $2"
    gppopt="-std=gnu++14"
  else
    echo "CC      $2"
    gppopt=""
  fi
  if [ $FORCE != tan ]; then
    if [ -e ${obj} ]; then
      fstime=`stat '--format=%Y' "$1/$2"`
      fotime=`stat '--format=%Y' ${obj}`
      if [ $fotime -ge $fstime ]; then
        return
      fi
    fi
  fi
  $GCC -pipe -c -O2 -Wall -Isrc/game ${gppopt} ${defines} ${cflags} -o ${obj} "$1/$2"
  if [ "$?" != "0" ]; then
    echo "FATAL: compilation failed!"
    exit 1
  fi
}


link() {
  echo "LINK    ivan${BINSUFF}"
  #echo "============="
  #echo "${objects}"
  #echo "============="
  #echo "${lflags}"
  #echo "============="
  $GPP -pipe -s -o ivan${BINSUFF} ${objects} ${lflags} ${linkflags}
  if [ "$?" != "0" ]; then
    echo "FATAL: linking failed!"
    exit 1
  fi
  ${ARCH}strip -s ivan${BINSUFF}
}


find_package sdl 1.2
if [ "$have_package" != "tan" ]; then
  echo "FATAL: you need SDL development package installed, version 1.2 or above!"
  exit 1
fi

find_package SDL_mixer 1.2
if [ "$have_package" != "tan" ]; then
  echo "MSG: no SDL_mixer package found, sound disabled"
  defines="${defines} -DDISABLE_SOUND"
else
  echo "MSG: sound support enabled"
fi

find_package libpng 1.6
if [ "$have_package" = "tan" ]; then
  defines="${defines} -DHAVE_LIBPNG"
  echo "MSG: libpng found"
else
  find_package libpng16 1.6
  if [ "$have_package" = "tan" ]; then
    defines="${defines} -DHAVE_LIBPNG"
    echo "MSG: libpng found"
  else
    #find_package imlib2 1.4
    #if [ "$have_package" = "tan" ]; then
    #  defines="${defines} -DHAVE_IMLIB2"
    #  echo "MSG: imlib2 found"
    #fi
    echo "FATAL: libpng v1.6 not found!"
    exit 1
  fi
fi

find_package zlib 1.2
if [ "$have_package" = "tan" ]; then
  defines="${defines} -DUSE_ZLIB"
  echo "MSG: compressed saves enabled"
fi

find_package gl
if [ "$have_package" = "tan" ]; then
  echo "MSG: OpenGL renderer enabled"
else
  echo "MSG: OpenGL renderer disabled"
  defines="${defines} -DDISABLE_OPENGL"
fi

find_package alsa
if [ "$have_package" = "tan" ]; then
  echo "MSG: ALSA silencing enabled"
  defines="${defines} -DENABLE_ALSA"
fi

#echo "CFLAGS: ${cflags}"
#echo "DEFINES: ${defines}"

mkdir ${BUILD_DIR} 2>/dev/null

compile src/felib bitmap.cpp
compile src/felib config.cpp
compile src/felib feerror.cpp
compile src/felib feio.cpp
compile src/felib felist.cpp
compile src/felib femain.cpp
compile src/felib femath.cpp
compile src/felib festring.cpp
compile src/felib fetime.cpp
compile src/felib graphics.cpp
compile src/felib hscore.cpp
compile src/felib rawbit.cpp
compile src/felib fesave.cpp
compile src/felib feparse.cpp
compile src/felib whandler.cpp
compile src/felib regex.c

compile src/game ivancommon.cpp
compile src/game actset.cpp
compile src/game areaset.cpp
compile src/game charset.cpp
compile src/game command.cpp
compile src/game coreset.cpp
compile src/game dataset.cpp
compile src/game dungeon.cpp
compile src/game game.cpp
compile src/game godset.cpp
compile src/game iconf.cpp
compile src/game id.cpp
compile src/game igraph.cpp
compile src/game itemset.cpp
compile src/game levelset.cpp
compile src/game main.cpp
compile src/game materset.cpp
compile src/game message.cpp
compile src/game object.cpp
compile src/game roomset.cpp
compile src/game script.cpp
compile src/game slotset.cpp
compile src/game trapset.cpp
compile src/game wmapset.cpp
compile src/game wskill.cpp


link
