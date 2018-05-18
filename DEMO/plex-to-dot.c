#include "phorward.h"

int main (int argc, char** argv)
{
  int i;
  plex *s;
  pregex_ptn *err_;
  int err;
  FILE* f;

  enum
  {
    ID1 = 1,
    ID2 = 2,
    ID3 = 3,
    ID4 = 4,
    ID5 = 5,
    ID6 = 6,
    ID7 = 7
  };

  struct
  {
    char* pat;
    int id;
    int flags;
  } patterns[] =
  {
    {"wstring",ID1,PREGEX_COMP_STATIC},
    {"zstring",ID2,PREGEX_COMP_STATIC},
    {"string",ID3,PREGEX_COMP_STATIC},
    {"wstr",ID4,PREGEX_COMP_STATIC},
    {"str",ID5,PREGEX_COMP_STATIC},
    {"bling",ID6,PREGEX_COMP_STATIC},
    {"wing",ID7,PREGEX_COMP_STATIC},
    {"keywords.dot",0,-1},
    {"_[a-zA-Z0-9_]+",ID1,0},
    {"[a-zA-Z][a-zA-Z0-9_]*",ID1,0},
    {"identifier.dot",0,-1},
    {"abc.*def.*ghi",ID1,0},
    {"large.dot",0,0},
    {"",-1,-1}
  };

  i = 0;
  while (1)
  {
    s =  plex_create(0);
    if (s == 0)
    {
      fprintf(stderr,"Could not create lexical analyzer, exiting");
      return -1;
    };
    while (patterns[i].id != 0)
    {
      err_ = plex_define(s,patterns[i].pat,patterns[i].id,patterns[i].flags);
      if (err_ == 0)
      {
        fprintf(stderr,"Problem while creating pattern, exiting");
        plex_free(s);
        return -1;
      };
      i++;
    };
    err = plex_prepare(s);
    if (err == 0)
    {
      fprintf(stderr,"Could not generate dfa");
      plex_free(s);
      return -1;
    };

    f = fopen( patterns[i].pat, "wt" );
    plex_dump_dot( f, s );
    fclose( f );

    plex_free(s);
    i++;
    if (patterns[i].id == -1 && patterns[i].flags == -1)
      return 0;
  };
};
