#include "phorward.h"

#define SIZE (lex->trans[i][0])
#define FROM (lex->trans[i][j])
#define TO   (lex->trans[i][j+1])
#define DST  (lex->trans[i][j+2])
#define MATCH_FLAGS (lex->trans[i][2])
#define REF_FLAGS (lex->trans[i][3])
#define FINAL_STATE (lex->trans[i][1] > 0)
#define ID (lex->trans[i][1])
#define DEFAULT_TRANSITION (lex->trans[i][4] != lex->trans_cnt)

void write_edge(FILE* transitions, wchar_t from, wchar_t to);
int write_dfa(plex *lex, char* filename);

void write_edge(FILE* transitions, wchar_t from, wchar_t to)
{
  if (iswprint(from))
    fprintf(transitions,"&#x%x;",from);
  else
    fprintf(transitions,"0x%x",from);
  if (to != from)
  {
    if (iswprint(to))
      fprintf(transitions," - &#x%x;",to);
    else
      fprintf(transitions," - 0x%x",to);
  };
};

int write_dfa(plex *lex, char* filename)
{
  int i,j;
  FILE *transitions;
  int dst;

  if (strlen(filename) == 0)
    return -1;
  transitions = fopen(filename,"w+b");
  if (transitions == 0)
    return -1;

  /* write start of graph */
  fprintf(transitions,"digraph {\n");
  fprintf(transitions,"  rankdir=LR;\n");
  fprintf(transitions,"  node [shape = circle];\n");

  for (i = 0;i < lex->trans_cnt;i++)
  {
    /* size = lex->trans[i][0]; */
    fprintf(transitions,"  n%d [",i);
    /* change shape and label of node if state is a final state */
    if (FINAL_STATE)
      fprintf(transitions,"shape=doublecircle,");
    fprintf(transitions,"label = \" n%d\\nmatch_flags = %d\\nref_flags = %d\\n",i,MATCH_FLAGS,REF_FLAGS);
    if (FINAL_STATE)
      fprintf(transitions,"id = %d\\n", ID);
    fprintf(transitions,"\"];\n");
    /* default transition */
    if (DEFAULT_TRANSITION)
      fprintf(transitions,"  n%d -> n%d [style=bold];\n",i,lex->trans[i][4]);
    /* a state with size < 5 is a final state: it has no outgoing transitions */
    if (SIZE > 5)
    {
      j = 5;
      while (1)
      {
        fprintf(transitions,"  n%d -> n%d [label = <",i,DST);
        write_edge(transitions,FROM,TO);
        dst = lex->trans[i][j+2];
        j += 3;
        while (1)
        {
          /*  no more transitions to write */
          if (j >= SIZE)
          {
            fprintf(transitions,">];\n");
            break;
          };
          if (lex->trans[i][j+2] == dst)
          {
            fprintf(transitions,"<br/>");
            write_edge(transitions,FROM,TO);
            j += 3;
            continue;
          };
          /* no more transitions to write */
          fprintf(transitions,">];\n");
          break;
        };
        if (j >= SIZE)
          break;
      };
    };
  };
  fprintf(transitions,"}\n");
  fclose(transitions);
  return 0;
};

int main (int argc, char** argv)
{
  int i;
  plex *s;
  pregex_ptn *err_;
  int err;

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
    err = write_dfa(s,patterns[i].pat);
    if (err)
    {
      fprintf(stderr,"Could not create file %s",patterns[i].pat);
      plex_free(s);
      return -1;
    };
    plex_free(s);
    i++;
    if (patterns[i].id == -1 && patterns[i].flags == -1)
      return 0;
  };
};
