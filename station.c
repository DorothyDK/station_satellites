#include <stdio.h>
#include <string.h>
#include <libgen.h>

#define msta 100000
#define msat 128
#define mlen 512

const char fnini[]="f4_n.ini";
double t1,t2,lat1,lat2,lon1,lon2;
int nsats=0,nsta=0, nst=0,nstr9; 
typedef char s4[8];
s4 st[msta];
typedef char s2[8];
s2 sats[msat];
struct lola { char n[8]; double lo; double la; };
struct lola sta[msta];
char dir[mlen], day[mlen], station[4], flname[256], fnsta[mlen];

// массив спутников 
int rdsats(char *s, int len){
   int i,j;
   i=0;  bzero(sats,sizeof(sats));
   for (nsats=0; ; nsats++ ) {
    for (; (i<len)&&(s[i]==' '); i++); 
    if ((i==len)||(s[i]==0x0A)||(s[i]==0x0D)) break;
    j=strcspn(&s[i]," "); 
    strncpy(sats[nsats],&s[i],j);
    i+=j;
  } 
  return(0);
}

// массив станций
int rdst(char *s, int len){
   int i,j;
   i=0;  
   for (nst=0; ; nst++ ) {
    for (; (i<len)&&(s[i]==' '); i++); 
    if ((i==len)||(s[i]==0x0A)||(s[i]==0x0D)) break;
    j=strcspn(&s[i]," "); 
    strncpy(st[nst],&s[i],j);
    i+=j;
  } 
  return(0);
}

// чтение станций из файла sitecoor.dat и запись файла station.dat
int rdsta() {
  int i,rc,k;
  double x,y,z;
  char  s[mlen];
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
      for (k=0; k<nst; k++) {
        if (strncasecmp(st[k],sta[nsta].n,4)==0) {fprintf(f,"%s %lf %lf\n",sta[nsta].n, sta[nsta].la, sta[nsta].lo);}
      }
    fgets(s,mlen,fsit);
    if (rc==EOF) break;
    if (rc!=6) break;    
    nsta++;
   }
   
  fclose(fsit);
  fclose(f);
  if (rc!=EOF) { printf("rdsta, file \"%s\" read error\n",fnsta); return(rc); }
  return(0);
}

// чтение ini файла
int rdini() {
  const char stime1[]="time1", stime2[]="time2", ssats[]="satellites", sts[]="station",
  slon1[]="lon1", slon2[]="lon2", slat1[]="lat1", slat2[]="lat2", sdir[]="dir",
  sdoy[]="doy", snstr9[]="nstr9", sfnsta[]="fnsta";
  char  s[mlen],*ps;
  int ft1=0,ft2=0;
  FILE  *fini;
  bzero(sats,sizeof(sats)); nstr9=0; 
  
  if ((fini=fopen(fnini,"r"))==NULL) {
    printf("rdini, file \"%s\" not found\n",fnini);
    return(1);
  }
  
  while ((ps=fgets(s,mlen,fini))!=NULL) {
    if (strlen(s)==1) continue;
    if (s[0]=='#') continue;
    if (strncasecmp(s,sfnsta,5)==0) sscanf(&s[6],"%s",fnsta);
    if (strncasecmp(s,sdir,3)==0)   sscanf(&s[4],"%s",dir);
    if (strncasecmp(s,snstr9,5)==0) sscanf(&s[6],"%d",&nstr9);
    if (strncasecmp(s,stime1,5)==0) { sscanf(&s[6],"%lf",&t1); ft1=1; }
    if (strncasecmp(s,stime2,5)==0) { sscanf(&s[6],"%lf",&t2); ft2=1; }
    if (strncasecmp(s,ssats,10)==0) rdsats(&s[11],strlen(s)-11); 
    if (strncasecmp(s,sts,7)==0)    rdst(&s[8],strlen(s)-8); 
    if (strncasecmp(s,slon1,4)==0)  sscanf(&s[5],"%lf",&lon1);
    if (strncasecmp(s,slon2,4)==0)  sscanf(&s[5],"%lf",&lon2);
    if (strncasecmp(s,slat1,4)==0)  sscanf(&s[5],"%lf",&lat1);
    if (strncasecmp(s,slat2,4)==0)  sscanf(&s[5],"%lf",&lat2);
    if (strncasecmp(s,sdoy,3)==0)   sscanf(&s[4],"%s",day);
  }
  fclose(fini);
  if ((ft1==0)||(ft2==0)) { 
    printf("rdini, parameter isn't set\n"); return(1);
  }
  return(0);
}

// формирование имен файлов, запись файла datanew.dat
int rddat() {
    char s[mlen]; 
    int i,n,m,rc;
    int last_ti;
    double ti,ga,az,la,lo,tec,tecf;
    FILE *fdat, *newdat;
    newdat=fopen("datanew.dat","a");
    for (n=0; n<nst; n++){
        strncpy(station,st[n],4);
        for (m=0; m<nsats; m++){
            sprintf(flname,"%s%s/%s%s_%s.dat",dir, station,station,sats[m],day );
            if ((fdat=fopen(flname,"r"))!=NULL) {
            printf("rddat, file \"%s\" found\n",flname);
                for (i=0; i<nstr9; i++) fgets(s,mlen,fdat);
                while (1) {
                    rc=fscanf(fdat,"%lf %lf %lf %lf %lf %lf %lf\n",&ti,&ga,&az,&la,&lo,&tec,&tecf);
                    fgets(s,mlen,fdat);
                    if (rc!=7) break; 
                    if (ti>=t2) {rc=EOF; break;}
                
                    if (ti-last_ti>1) fprintf(newdat,"\n");
               
                    fprintf(newdat, "%lf %lf %lf\n", ti, la, lo);
                    last_ti = ti;
                }
                fprintf(newdat, "\n");
                fclose(fdat);
            }
        }
    }
    fclose(newdat); 
    if (rc!=EOF) { printf("rddat, read error file %s\n",flname); return(rc); }
    return(0);
}

int main(int argc, char* argv[]) {
rdini(); 
rdsta();
rddat();
return(0);  
}
