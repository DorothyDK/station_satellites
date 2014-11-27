#include <stdio.h>
#include <string.h>


#define msta 100000
#define msat 128
#define mlen 512

const char fnini[]="f4_n.ini";                            
const char fnsta[]="sitecoor.dat";
double t1,t2,lat1=-361,lat2=361,lon1=-361,lon2=361;
int nsat=0,sats[msat+1],nsta=0,ffsta=1, nst=0; 
//struct namest { char m[4]; };
//struct namest st[msta];
typedef char s4[8];
s4 st[msta];
struct lola { char n[8]; double lo; double la; };
struct lola sta[msta];

int rdsats(char *s, int len) {
  int i,j;
  i=0; nsat=0;
  while (i<len) {
    sscanf(&s[i],"%d",&j); if (j>0) { sats[j]=1; nsat++; }
    while ((i<len)&&(s[i]!=' ')) i++;
    i++;
  }  
  return(0);
}

int rdst(char *s, int len) {
  int i;
  i=0; nst=0;
  while (i<len) {
    strncpy(st[nst],&s[i],4);
    i=i+5;
    nst++;
  } 
  nst=nst-1;
//  printf("%s\n",st[0]);
  return(0);
}

int rdsta() {
  int i,rc,k;
  double x,y,z;
  char	s[mlen];
  FILE *fsit, *f;
  f=fopen("station.dat","w");
  if ((fsit=fopen(fnsta,"r"))==NULL) {
    printf("rdsta, file \"%s\" not found\n",fnsta);
    return(1);
  }  

  for (i=0; i<3; i++) fgets(s,mlen,fsit);
   while (1) {
    rc=fscanf(fsit,"%s %lf %lf %lf %lf %lf",
      sta[nsta].n,&x,&y,&z,&sta[nsta].la,&sta[nsta].lo);
   for (k=0; k<=nst; k++) {
 //  if (strcmp(st[k],sta[nsta].n)==0) printf("%s\n",sta[nsta].n);
   if (strncasecmp(st[k],sta[nsta].n,4)==0) {fprintf(f,"%s %lf %lf\n",sta[nsta].n, sta[nsta].la, sta[nsta].lo);}
   }
    fgets(s,mlen,fsit);
//   printf("%s\n",st[nst]);
    if (rc==EOF) break;
    if (rc!=6) break;    
    nsta++;
}

  fclose(fsit);
  fclose(f);
  if (rc!=EOF) { printf("rdsta, file \"%s\" read error\n",fnsta); return(rc); }
  return(0);
}

int rdini() {
  const char stime1[]="time1", stime2[]="time2", ssats[]="satellites", st[]="station",
   slon1[]="lon1", slon2[]="lon2", slat1[]="lat1", slat2[]="lat2";
  char  s[mlen],*ps;
  int ft1=0,ft2=0;
  FILE  *fini;
  bzero(sats,sizeof(sats));
  
  if ((fini=fopen(fnini,"r"))==NULL) {
    printf("rdini, file \"%s\" not found\n",fnini);
    return(1);
  }
  
  while ((ps=fgets(s,mlen,fini))!=NULL) {
    if (strlen(s)==1) continue;
    if (s[0]=='#') continue;
    if (strncasecmp(s,stime1,5)==0) { sscanf(&s[6],"%lf",&t1); ft1=1; }
    if (strncasecmp(s,stime2,5)==0) { sscanf(&s[6],"%lf",&t2); ft2=1; }
    if (strncasecmp(s,ssats,10)==0) rdsats(&s[11],strlen(s)-11); 
    if (strncasecmp(s,st,7)==0)     rdst(&s[8],strlen(s)-8); 
    if (strncasecmp(s,slon1,4)==0) sscanf(&s[5],"%lf",&lon1);
    if (strncasecmp(s,slon2,4)==0) sscanf(&s[5],"%lf",&lon2);
    if (strncasecmp(s,slat1,4)==0) sscanf(&s[5],"%lf",&lat1);
    if (strncasecmp(s,slat2,4)==0) sscanf(&s[5],"%lf",&lat2);
 // printf("%s ",s);
  }
//  printf("satellites: %lf ",lon2);
  fclose(fini);
  if ((ft1==0)||(ft2==0)) { 
    printf("rdini, parameter isn't set\n"); return(1);
  }
  if ((lon1==-361)&&(lon2==361)&&(lat1==-361)&&(lat2==361)) ffsta=0;
  return(0);
}

int rddat(char *fn/*, char *tmp*/) {
  char s[mlen];                                
  int i,rc;
  double ti,ga,az,la,lo,tec,tecf;
 // const char fn[]="SUWN02_070.dat";
  FILE *fdat, *newdat;
 // strcpy(s,tmp); strcat(s,"res.dat");
  newdat=fopen("datanew.dat","a");
   if ((fdat=fopen(fn,"r"))==NULL) {
    printf("rddat, file \"%s\" not found\n",fn);
    return(2);
  }
 // fprintf(newdat,"time,  latp,  lonp\n");
   for (i=0; i<9; i++) fgets(s,mlen,fdat);
    while (1) {
     rc=fscanf(fdat,"%lf %lf %lf %lf %lf %lf %lf\n",&ti,&ga,&az,&la,&lo,&tec,&tecf);
     fgets(s,mlen,fdat);
     if (rc!=7) break; 
     if (ti>=t2) break;
     if (ti<t1) continue;
     fprintf(newdat, "%lf %lf %lf\n", ti, la, lo);
    }
  fprintf(newdat, "\n");

  fclose(fdat); fclose(newdat); 
  if (rc!=EOF) { printf("rddat, read error file %s\n",fn); return(rc); }
  return(0);
}

int main(int argc, char* argv[]) {
rdini(); 
rdsta();
rddat(argv[1]/*,argv[2]*/);
return(0);  
}
