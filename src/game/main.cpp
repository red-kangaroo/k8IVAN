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
#include "ivancommon.h"

#include <iostream>

#include "game.h"
#include "database.h"
#include "feio.h"
#include "igraph.h"
#include "iconf.h"
#include "whandler.h"
#include "hscore.h"
#include "graphics.h"
#include "script.h"
#include "message.h"
#include "proto.h"


////////////////////////////////////////////////////////////////////////////////
#define PUSH_BACK(_c)  (*ress)[dpos++] = (_c)
#define DECODE_TUPLE(tuple,bytes) \
  for (tmp = bytes; tmp > 0; tmp--, tuple = (tuple & 0x00ffffff)<<8) \
    PUSH_BACK((char)((tuple >> 24)&0xff))

// returns ress length
static int ascii85Decode (char **ress, const char *srcs/*, int start, int length*/) {
  static uint32_t pow85[5] = { 85*85*85*85UL, 85*85*85UL, 85*85UL, 85UL, 1UL };
  const uint8_t *data = (const uint8_t *)srcs;
  int len = strlen(srcs);
  uint32_t tuple = 0;
  int count = 0, c = 0;
  int dpos = 0;
  int start = 0, length = len;
  int tmp;
  //
  if (start < 0) start = 0; else { len -= start; data += start; }
  if (length < 0 || len < length) length = len;
  /*
  if (length > 0) {
    int xlen = 4*((length+4)/5);
    kstringReserve(ress, xlen);
  }
  */
  //
  *ress = (char *)calloc(1, len+1);
  for (int f = length; f > 0; --f, ++data) {
    c = *data;
    if (c <= ' ') continue; // skip blanks
    switch (c) {
      case 'z': // zero tuple
      if (count != 0) {
        //xdlog("%s: z inside ascii85 5-tuple\n", file);
        free(*ress);
        *ress = NULL;
        return -1;
      }
      PUSH_BACK('\0');
      PUSH_BACK('\0');
      PUSH_BACK('\0');
      PUSH_BACK('\0');
      break;
    case '~': // '~>': end of sequence
      if (f < 1 || data[1] != '>') { free(*ress); return -2; } // error
      if (count > 0) { f = -1; break; }
    default:
      if (c < '!' || c > 'u') {
        //xdlog("%s: bad character in ascii85 region: %#o\n", file, c);
        free(*ress);
        return -3;
      }
      tuple += ((uint8_t)(c-'!'))*pow85[count++];
      if (count == 5) {
        DECODE_TUPLE(tuple, 4);
        count = 0;
        tuple = 0;
      }
      break;
    }
  }
  // write last (possibly incomplete) tuple
  if (count-- > 0) {
    tuple += pow85[count];
    DECODE_TUPLE(tuple, count);
  }
  return dpos;
}

#undef PUSH_BACK
#undef DECODE_TUPLE


static void decodeBA (char *str, int len) {
  char pch = 42;
  //
  for (int f = 0; f < len; ++f, ++str) {
    char ch = *str;
    //
    ch = (ch-f-1)^pch;
    *str = ch;
    pch = ch;
  }
}


static void printEC (const char *txt) {
  char *dest;
  int len;
  //
  if ((len = ascii85Decode(&dest, txt)) >= 0) {
    decodeBA(dest, len);
    fprintf(stderr, "%s\n", dest);
    free(dest);
  }
}


static int isStr85Equ (const char *txt, const char *str) {
  char *dest;
  int len, res = 0;
  //
  if ((len = ascii85Decode(&dest, txt)) >= 0) {
    res = (strcmp(dest+1, str) == 0); // +1 to ignore '/'
    free(dest);
  }
  return res;
}


static int checkEGG (const char *str) {
  if (isStr85Equ("06:]JASq", str) || isStr85Equ("0/i", str)) {
    printEC(
      "H8lZV&6)1>+AZ>m)Cf8;A1/cP+CnS)0OJ`X.QVcHA4^cc5r3=m1c%0D3&c263d?EV6@4&>"
      "3DYQo;c-FcO+UJ;MOJ$TAYO@/FI]+B?C.L$>%:oPAmh:4Au)>AAU/H;ZakL2I!*!%J;(AK"
      "NIR#5TXgZ6c'F1%^kml.JW5W8e;ql0V3fQUNfKpng6ppMf&ip-VOX@=jKl;#q\"DJ-_>jG"
      "8#L;nm]!q;7c+hR6p;tVY#J8P$aTTK%c-OT?)<00,+q*8f&ff9a/+sbU,:`<H*[fk0o]7k"
      "^l6nRkngc6Tl2Ngs!!P2I%KHG=7n*an'bsgn>!*8s7TLTC+^\\\"W+<=9^%Ol$1A1eR*Be"
      "gqjEag:M0OnrC4FBY5@QZ&'HYYZ#EHs8t4$5]!22QoJ3`;-&=\\DteO$d6FBqT0E@:iu?N"
      "a5ePUf^_uEEcjTDKfMpX/9]DFL8N-Ee;*8C5'WgbGortZuh1\\N0;/rJB6'(MSmYiS\"6+"
      "<NK)KDV3e+Ad[@).W:%.dd'0h=!QUhghQaNNotIZGrpHr-YfEuUpsKW<^@qlZcdTDA!=?W"
      "Yd+-^`'G8Or)<0-T&CT.i+:mJp(+/M/nLaVb#5$p2jR2<rl7\"XlngcN`mf,[4oK5JLr\\"
      "m=X'(ue;'*1ik&/@T4*=j5t=<&/e/Q+2=((h`>>uN(#>&#i>2/ajK+=eib1coVe3'D)*75"
      "m_h;28^M6p6*D854Jj<C^,Q8Wd\"O<)&L/=C$lUAQNN<=eTD:A6kn-=EItXSss.tAS&!;F"
      "EsgpJTHIYNNnh'`kmX^[`*ELOHGcWbfPOT`J]A8P`=)AS;rYlR$\"-.RG440lK5:Dg?G'2"
      "['dE=nEm1:k,,Se_=%-6Z*L^J[)EC"
    );
    return 1;
  }
  if (isStr85Equ("04Jj?B)", str)) {
    printEC(
      "IPaSa(`c:T,o9Bq3\\)IY++?+!-S9%P0/OkjE&f$l.OmK'Ai2;ZHn[<,6od7^8;)po:HaP"
      "m<'+&DRS:/1L7)IA7?WI$8WKTUB2tXg>Zb$.?\"@AIAu;)6B;2_PB5M?oBPDC.F)606Z$V"
      "=ONd6/5P*LoWKTLQ,d@&;+Ru,\\ESY*rg!l1XrhpJ:\"WKWdOg?l;=RHE:uU9C?aotBqj]"
      "=k8cZ`rp\"ZO=GjkfD#o]Z\\=6^]+Gf&-UFthT*hN"
    );
    return 1;
  }
  if (isStr85Equ("04o69A7Tr", str)) {
    printEC(
      "Ag7d[&R#Ma9GVV5,S(D;De<T_+W).?,%4n+3cK=%4+0VN@6d\")E].np7l?8gF#cWF7SS_m"
      "4@V\\nQ;h!WPD2h#@\\RY&G\\LKL=eTP<V-]U)BN^b.DffHkTPnFcCN4B;]8FCqI!p1@H*_"
      "jHJ<%g']RG*MLqCrbP*XbNL=4D1R[;I(c*<FuesbWmSCF1jTW+rplg;9[S[7eDVl6YsjT"
    );
    return 1;
  }
  return 0;
}


int Main (int argc, char *argv[]) {
  for (int f = 1; f < argc; ++f) {
    if (checkEGG(argv[f]+1)) exit(1);
  }
  //
  if (argc > 1 && (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v"))) {
    std::cout << "Iter Vehemens ad Necem version " << IVAN_VERSION << std::endl;
    return 0;
  }
  //
  //femath::SetSeed(time(0));
  femath::RandSeed();
  game::InitGlobalValueMap();
  scriptsystem::Initialize();
  databasesystem::Initialize();
  game::InitLuxTable();
  ivanconfig::Initialize();
  igraph::Init();
  game::CreateBusyAnimationCache();
  globalwindowhandler::SetQuitMessageHandler(game::HandleQuitMessage);
  msgsystem::Init();
  protosystem::Initialize();
  igraph::LoadMenu();
  //
  for(;;) {
    int Select = iosystem::Menu(
      igraph::GetMenuGraphic(),
      v2(RES.X / 2, RES.Y / 2 - 20),
      CONST_S("\r"),
      CONST_S("Start Game\rContinue Game\rConfiguration\rHighscores\rQuit\r"),
      LIGHT_GRAY,
      CONST_S("Released under the GNU\rGeneral Public License\rMore info: see COPYING\r"),
      CONST_S("IVAN v" IVAN_VERSION "\r"),
      true);
    switch (Select) {
      case -1: return 0; // esc
      case 0:
        if (game::Init()) {
          igraph::UnLoadMenu();
          game::Run();
          game::DeInit();
          igraph::LoadMenu();
        }
        break;
      case 1: {
        festring LoadName = iosystem::ContinueMenu(WHITE, LIGHT_GRAY, game::GetSavePath());
        if (LoadName.GetSize()) {
          LoadName.Resize(LoadName.GetSize()-4);
          if (game::Init(LoadName)) {
            igraph::UnLoadMenu();
            game::Run();
            game::DeInit();
            igraph::LoadMenu();
          }
        }
        break; }
      case 2:
        ivanconfig::Show();
        break;
      case 3: {
        highscore HScore;
        HScore.Draw();
        break; }
      case 4: return 0;
    }
  }
}
