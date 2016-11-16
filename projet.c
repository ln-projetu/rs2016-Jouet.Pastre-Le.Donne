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


long long convertOctalToDecimal(int octalNumber){
  int decimalNumber = 0, i = 0;
  while(octalNumber != 0) {
    decimalNumber += (octalNumber%10) * pow(8,i);
    ++i;
    octalNumber/=10;
  }
  i = 1;
  return decimalNumber;
}

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
  int fd = open(argv[argc-1],O_RDONLY);
  struct header_posix_ustar ma_struct;
  struct stat stats;
  struct utimbuf utimbuf;
  struct timeval times[2];
  struct timeval const *tval;
  struct tm ts;
  //struct timespec times[2];
  int i=0;
  while(read(fd,&ma_struct,512)!=0) {
    i+=1;
    if(strcmp(ma_struct.name,"\0")!=0) {
      printf("%s",ma_struct.name);
      //write(1,&ma_struct,100);
      printf("\n");
    }
    int j=convertOctalToDecimal(atoi(ma_struct.size));
    lseek(fd,512*arrondi512(j),SEEK_CUR);
  }
  close(fd);

  while((opt=getopt(argc,argv,optstring))!=EOF) {
    switch (opt){
      case 'x':
        printf("\n");
        int fdd = open(argv[argc-1],O_RDONLY);
        while(read(fdd,&ma_struct,512)!=0) {
          int m=convertOctalToDecimal(atoi(ma_struct.mode));

           if (strcmp(ma_struct.typeflag,"5")==0){
              mkdir(ma_struct.name,(mode_t)m);
            }
            if(strcmp(ma_struct.typeflag,"0")==0) {
              creat(ma_struct.name,m);
            }
            if(ma_struct.typeflag[0]=='2') {
              symlink(ma_struct.linkname, ma_struct.name);
            }
            int j=convertOctalToDecimal(atoi(ma_struct.size));
            lseek(fdd,512*arrondi512(j),SEEK_CUR);
        }
        close(fdd);


        int fdcont = open(argv[argc-1],O_RDONLY);
        while(read(fdcont,&ma_struct,512)!=0) {
          int j=convertOctalToDecimal(atoi(ma_struct.size));
          if(strcmp(ma_struct.typeflag,"0")==0) {
            char buffer[j];
            int fdfichier = open(ma_struct.name,O_WRONLY);
            read(fdcont,buffer,j);
            write(fdfichier,buffer,j);
            close(fdfichier);
            }
            lseek(fdcont,(512*arrondi512(j))-j,SEEK_CUR);
        }
        close(fdcont);

       int fdtime = open(argv[argc-1],O_RDONLY);
        while(read(fdtime,&ma_struct,512)!=0) {
          int j=(int) strtol(ma_struct.size,NULL,8);
          int mt=(int) strtol(ma_struct.mtime,NULL,8);
          //char buf[80];
          //time_t tsec;
          //struct tm ts;
          time_t tt=(time_t)mt;
          //ts=*localtime(&tt);
          //printf("d");
          //strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",&ts);
          //printf("%s\n",buf);
          if(strcmp(ma_struct.typeflag,"0")==0) {
            int fdfi = open(ma_struct.name,O_WRONLY);
            int fds=fstat(fdfi,&stats);
            utimbuf.actime=stats.st_atime;
            utimbuf.modtime=tt;
            int fdutime=utime(ma_struct.name,&utimbuf);
            printf("%d\n",fdutime);
            //int a=(int)stats.st_mtime;
            //printf("stmtime: %d\n",a);
            /*int b=(int)tt;
            printf("tt: %d\n",b);
            stats.st_ctime=tt;
            int c=(int)stats.st_ctime;
            printf("stmtime: %d\n",c);*/
            close(fdfi);
            close(fds);
            close(fdutime);
          }
          if(ma_struct.typeflag[0]=='2') {
            int fds=lstat(ma_struct.name,&stats);
            times[0].tv_sec=(long)stats.st_atime;
            times[0].tv_usec=0;
            times[1].tv_sec=(long)tt;
            times[1].tv_usec=0;
            tval=times;
            int fdutime=lutimes(ma_struct.name,tval);
            close(fds);
            close(fdutime);
          }
          if(strcmp(ma_struct.typeflag,"5")==0) {
            int fds=stat(ma_struct.name,&stats);
            utimbuf.actime=stats.st_atime;
            utimbuf.modtime=tt;
            int fdutime=utime(ma_struct.name,&utimbuf);
          }
            lseek(fdtime,(512*arrondi512(j)),SEEK_CUR);
        }
        close(fdtime);

        break;

      case 'l':
        printf("\n");
        //printf("option l \n" );
        char date[80];
        char name[100];
        char mode[8];
        int uid;
        int gid;
        int size;
        int fdl = open(argv[argc-1],O_RDONLY);
        while(read(fdl,&ma_struct,512)!=0) {
          strcpy(mode,"mode");
          uid = (int) strtol(ma_struct.uid,NULL,8);
          gid = (int) strtol(ma_struct.gid,NULL,8);
          size = (int) strtol(ma_struct.size,NULL,8);
          int mt=(int) strtol(ma_struct.mtime,NULL,8);
          time_t tt=(time_t)mt;
          ts=*localtime(&tt);
          strftime(date,sizeof(date),"%Y-%m-%d %H:%M:%S",&ts);
          strcpy(name,ma_struct.name);
          printf("%s %d/%d %d %s %s\n",mode,uid,gid,size,date,name);
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

  return 0;
}
