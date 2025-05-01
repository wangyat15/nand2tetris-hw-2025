#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char *k,*v; } KV;
typedef struct { char *k; int v; } SV;

SV syms[] = {
    {"R0",0},{"R1",1},{"R2",2},{"R3",3},{"R4",4},{"R5",5},
    {"R6",6},{"R7",7},{"R8",8},{"R9",9},{"R10",10},{"R11",11},
    {"R12",12},{"R13",13},{"R14",14},{"R15",15},
    {"SP",0},{"LCL",1},{"ARG",2},{"THIS",3},{"THAT",4},
    {"SCREEN",16384},{"KBD",24576},{0}
};

KV comps[] = {
    {"-1","0111010"},{"0","0101010"},{"1","0111111"},
    {"D","0001100"},{"!D","0001101"},{"-D","0001111"},
    {"D+1","0011111"},{"D-1","0001110"},{"D&A","0000000"},{"D|A","0010101"},
    {"A","0110000"},{"!A","0110001"},{"-A","0110011"},{"A+1","0110111"},{"A-1","0110010"},
    {"D+A","0000010"},{"D-A","0010011"},{"A-D","0000111"},
    {"M","1110000"},{"!M","1110001"},{"-M","1110011"},{"M+1","1110111"},{"M-1","1110010"},
    {"D+M","1000010"},{"D-M","1010011"},{"M-D","1000111"},{"D&M","1000000"},{"D|M","1010101"},
    {0}
};
  
KV dests[] = {
    {"","000"},{"M","001"},{"D","010"},{"MD","011"},{"A","100"},{"AM","101"},{"AD","110"},{"AMD","111"},{0}
  };
  
KV jumps[] = {
    {"","000"},{"JGT","001"},{"JEQ","010"},{"JGE","011"},{"JLT","100"},{"JNE","101"},{"JLE","110"},{"JMP","111"},
    {0}
};
  
SV *st=NULL; int sc=0, va=16;

char* clean(char *l) {
  static char b[256]; int i=0,j=0;
  while(l[i]&&isspace(l[i])) i++;
  if(l[i]=='(') {
    b[j++]='('; i++;
    while(l[i]&&l[i]!=')'&&l[i]!='/') if(!isspace(l[i])) b[j++]=l[i++]; else i++;
    if(l[i]==')') b[j++]=')'; b[j]=0; return b;
  }
  while(l[i]&&l[i]!='/') if(!isspace(l[i])) b[j++]=l[i++]; else i++;
  b[j]=0; return b;
}

void add(char *s,int a) {
  st=realloc(st,(sc+1)*sizeof(SV));
  st[sc].k=strdup(s); st[sc++].v=a;
}

int find(char *s) {
  for(int i=0;syms[i].k;i++) if(!strcmp(s,syms[i].k)) return syms[i].v;
  for(int i=0;i<sc;i++) if(!strcmp(s,st[i].k)) return st[i].v;
  return -1;
}

char* get(KV *t,char *k) {
  for(int i=0;t[i].k;i++) if(!strcmp(t[i].k,k)) return t[i].v;
  return NULL;
}

int parse(FILE *f) {
  char buf[256],*ln; int pc=0;
  while(fgets(buf,256,f)) {
    ln=clean(buf);
    if(!ln[0]) continue;
    if(ln[0]=='('&&ln[strlen(ln)-1]==')') {
      char sym[256]; strncpy(sym,ln+1,strlen(ln)-2); sym[strlen(ln)-2]=0; add(sym,pc);
    } else pc++;
  }
  return pc;
}

void bin(int n,char *s,int w) {
  for(int i=w-1;i>=0;i--) s[w-1-i]=((n>>i)&1)?'1':'0'; s[w]=0;
}

void asm2bin(FILE *in,FILE *out) {
  char buf[256],*ln,bs[17]; int a;
  while(fgets(buf,256,in)) {
    ln=clean(buf);
    if(!ln[0]|| (ln[0]=='(' && ln[strlen(ln)-1]==')')) continue;
    if(ln[0]=='@') {
      char *v=ln+1; a=isdigit(*v)?atoi(v):(find(v)<0?(add(v,va),va++):find(v));
      bin(a,bs,16); fprintf(out,"0%s\n",bs+1);
    } else {
      char d[32]="",c[32]="",j[32]="";
      char *eq=strchr(ln,'='),*sc=strchr(ln,';');
      if(eq) {
        strncpy(d,ln,eq-ln); d[eq-ln]=0;
        if(sc) { strncpy(c,eq+1,sc-eq-1); c[sc-eq-1]=0; strcpy(j,sc+1); }
        else strcpy(c,eq+1);
      } else if(sc) { strncpy(c,ln,sc-ln); c[sc-ln]=0; strcpy(j,sc+1); }
      else strcpy(c,ln);
      char *cb=get(comps,c),*db=get(dests,d),*jb=get(jumps,j);
      if(!cb) { fprintf(stderr,"Error: Invalid comp '%s'\n",c); exit(1); }
      if(!db) db="000"; if(!jb) jb="000";
      fprintf(out,"111%s%s%s\n",cb,db,jb);
    }
  }
}

int main(int ac,char **av) {
  if(ac<2) { printf("Usage: %s file.asm [out.hack]\n",av[0]); return 1; }
  FILE *in=fopen(av[1],"r"); if(!in) { perror("Input open error"); return 1; }
  parse(in); rewind(in);
  char *outf;
  if(ac>2) outf=av[2];
  else {
    outf=malloc(strlen(av[1])+6);
    strcpy(outf,av[1]);
    char *dot=strrchr(outf,'.');
    if(dot && !strcmp(dot,".asm")) strcpy(dot,".hack");
    else strcat(outf,".hack");
  }
  FILE *out=fopen(outf,"w"); if(!out) { perror("Output open error"); fclose(in); return 1; }
  asm2bin(in,out);
  fclose(in); fclose(out);
  printf("Assembly complete. Output: %s\n",outf);
  if(ac<=2) free(outf);
  for(int i=0;i<sc;i++) free(st[i].k);
  free(st);
  return 0;
}
