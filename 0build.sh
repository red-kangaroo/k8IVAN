#!/bin/sh
# WARNING! SHITTY CODE!


defines="-DSGAME_SHOTS_IPU -DWIZARD -DLOCAL_SAVES"
cflags="-Wno-narrowing -Isrc/felib"
lflags="-lm"
objects=""


find_package() {
  local r
  #
  if [ "z$2" != "z" ]; then
    pkg-config --silence-errors $1 --atleast-version=$2
  else
    pkg-config --silence-errors $1
  fi
  r="$?"
  if [ "z$r" = "z0" ]; then
    have_package="tan"
    cflags="${cflags} `pkg-config $1 --cflags`"
    lflags="${lflags} `pkg-config $1 --libs`"
  else
    have_package="ona"
  fi
}


# $1: srcdir
# $2: srcfile
# $3: use gcc if not empty
compile() {
  local obj
  #
  obj=`basename $2 .cpp`
  obj=`basename $obj .c`
  obj="${obj}.o"
  echo "CC      $2"
  obj="_build/${obj}"
  objects="${objects} ${obj}"
  gcc -pipe -c -O2 -Wall ${defines} ${cflags} -o ${obj} "$1/$2"
  if [ "$?" != "0" ]; then
    echo "FATAL: compilation failed!"
    exit 1
  fi
}


link() {
  echo "LINK    ivan"
  g++ -pipe -s -o ivan ${objects} ${lflags}
  if [ "$?" != "0" ]; then
    echo "FATAL: linking failed!"
    exit 1
  fi
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
fi

find_package imlib2 1.4
if [ "$have_package" = "tan" ]; then
  defines="${defines} -DHAVE_IMLIB2"
fi

#find_package libpng 1.5
#if [ "$have_package" = "tan" ]; then
#  defines="${defines} -DHAVE_LIBPNG"
#fi


mkdir _build 2>/dev/null

compile src/felib bitmap.cpp
compile src/felib config.cpp
compile src/felib error.cpp
compile src/felib febot.cpp
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
compile src/felib whandler.cpp
compile src/felib regex.c

compile src/game actset.cpp
compile src/game areaset.cpp
compile src/game charset.cpp
compile src/game charsset.cpp
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