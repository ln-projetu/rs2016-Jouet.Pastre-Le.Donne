//main argument fichier -> récuperer la structure POSIX header, puis lire cette structure, lister
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
//#include <sys/time.h>
struct header_posix_ustar {
                   char name[100];
                   char mode[8];
                   char uid[8];
                   char gid[8];
                   char size[12];
                   char mtime[12];
                   char checksum[8];
                   char typeflag[1];
                   char linkname[100];
                   char magic[6];
                   char version[2];
                   char uname[32];
                   char gname[32];
                   char devmajor[8];
                   char devminor[8];
                   char prefix[155];
                   char pad[12];
           };

int arrondi512(int j) {
  int somme=0;
  int i512=0;
  while (somme<j){
    somme+=512;
    i512+=1;
  }
  return i512;
}

int main(int argc, char *argv[]) {
  int opt;
  char optstring[]="xlpz";

  struct header_posix_ustar ma_struct;
  struct stat stats;
  struct utimbuf utimbuf;
  struct timeval times[2];
  struct timeval const *tval;
  struct tm ts;
  int i=0;
  int lcase=0;



  while((opt=getopt(argc,argv,optstring))!=EOF) {
    int fdx;
    int fdl;
    switch (opt){
      case 'x':
        fdx = open(argv[argc-1],O_RDONLY);
        int m;
        while(read(fdx,&ma_struct,512)!=0) {
          m=(int) strtol(ma_struct.mode,NULL,8);
          if(ma_struct.typeflag[0]=='5') {
            mkdir(ma_struct.name,(mode_t)m);
          }
          if(ma_struct.typeflag[0]=='0') {
            creat(ma_struct.name,m);
          }
          if(ma_struct.typeflag[0]=='2') {
            symlink(ma_struct.linkname, ma_struct.name);
          }
          int size=(int) strtol(ma_struct.size,NULL,8);
          lseek(fdx,512*arrondi512(size),SEEK_CUR);
        }
        close(fdx);


        int fdcont = open(argv[argc-1],O_RDONLY);
        while(read(fdcont,&ma_struct,512)!=0) {
          int size=(int) strtol(ma_struct.size,NULL,8);
          if(ma_struct.typeflag[0]=='0') {
            char buffer[size];
            int fdfile = open(ma_struct.name,O_WRONLY);
            read(fdcont,buffer,size);
            write(fdfile,buffer,size);
            close(fdfile);
            }
            lseek(fdcont,(512*arrondi512(size))-size,SEEK_CUR);
        }
        close(fdcont);

       int fdtime = open(argv[argc-1],O_RDONLY);
        while(read(fdtime,&ma_struct,512)!=0) {
          int size=(int) strtol(ma_struct.size,NULL,8);
          int mt=(int) strtol(ma_struct.mtime,NULL,8);
          time_t tt=(time_t)mt;
          if(ma_struct.typeflag[0]=='0' || ma_struct.typeflag[0]=='5' ) {
            int fds=stat(ma_struct.name,&stats);
            utimbuf.actime=stats.st_atime;
            utimbuf.modtime=tt;
            int fdutime=utime(ma_struct.name,&utimbuf);
          }
          if(ma_struct.typeflag[0]=='2') {
            int fds=lstat(ma_struct.name,&stats);
            times[0].tv_sec=(long)stats.st_atime;
            times[0].tv_usec=0;
            times[1].tv_sec=(long)tt;
            times[1].tv_usec=0;
            tval=times;
            int fdutime=lutimes(ma_struct.name,tval);
          }
            lseek(fdtime,(512*arrondi512(size)),SEEK_CUR);
        }
        close(fdtime);
        break;

      case 'l':
        lcase=1;
        fdl = open(argv[argc-1],O_RDONLY);
        char date[80];
        char name[100];
        char mode[10];
        int uid;
        int gid;
        int size;
        int mt;
        int modei;
        int modedet[3];
        while(read(fdl,&ma_struct,512)!=0) {
          if(strcmp(ma_struct.name,"\0")!=0) {
            strcpy(mode,"mode");
            modei=(int)atoi(ma_struct.mode);
            modedet[0]=modei/100;
            modedet[1]=(modei-(modedet[0]*100))/10;
            modedet[2]=(modei-(modedet[0]*100+modedet[1]*10));
            if(ma_struct.typeflag[0]=='0') {
              strcpy(mode,"-");
            }
            if(ma_struct.typeflag[0]=='2') {
              strcpy(mode,"l");
            }
            if(ma_struct.typeflag[0]=='5') {
              strcpy(mode,"d");
            }
            int i;
            for (i=0; i<3; i++) {
              switch (modedet[i]){
                case 7:
                strcat(mode,"rwx");
                break;
                case 6:
                strcat(mode,"rw-");
                break;
                case 5:
                strcat(mode,"r-x");
                break;
                case 4:
                strcat(mode,"r--");
                break;
                case 3:
                strcat(mode,"-wx");
                break;
                case 2:
                strcat(mode,"-w-");
                break;
                case 1:
                strcat(mode,"--x");
                break;
                case 0:
                strcat(mode,"---");
                break;
              }
            }

            uid = (int) strtol(ma_struct.uid,NULL,8);
            gid = (int) strtol(ma_struct.gid,NULL,8);
            size = (int) strtol(ma_struct.size,NULL,8);
            mt=(int) strtol(ma_struct.mtime,NULL,8);
            time_t tt=(time_t)mt;
            ts=*localtime(&tt);
            strftime(date,sizeof(date),"%Y-%m-%d %H:%M:%S",&ts);
            strcpy(name,ma_struct.name);
            printf("%s %d/%d %d %s %s\n",mode,uid,gid,size,date,name);
            }
          lseek(fdl,(512*arrondi512(size)),SEEK_CUR);
          }
        close(fdl);
        break;

      case 'p':
        //printf("option p \n");
        break;
        
      case 'z':
        //printf("option z \n");
        break;
    }
  }

  if (lcase==0) {
    int fd = open(argv[argc-1],O_RDONLY);
    while(read(fd,&ma_struct,512)!=0) {
      if(strncmp(ma_struct.magic,"ustar",5)==0) {
        printf("%s\n",ma_struct.name);
      }
      int size=(int) strtol(ma_struct.size,NULL,8);
      lseek(fd,512*arrondi512(size),SEEK_CUR);
    }
    close(fd);
  }
  return 0;
}
