//main argument fichier -> récuperer la structure POSIX header, puis lire cette structure, lister
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
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

               while(octalNumber != 0)
               {
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
  //char* suite=NULL;
  //char buf=NULL;
  char optstring[]="xlpz";
  //char test[512];
  int fd = open(argv[argc-1],O_RDONLY);
  //int fd2 = open("header.txt",O_WRONLY);
  struct header_posix_ustar ma_struct;
  int i=0;
  while(read(fd,&ma_struct,512)!=0) {
    //read(fd,&ma_struct,512);
    //write(fd2,&ma_struct,512);
    //printf("%s\n",ma_struct.magic);
    i+=1;
  //  printf("bloc %d\n",i);
    if(strcmp(ma_struct.name,"\0")!=0) {
      printf("%s",ma_struct.name);
      //write(1,&ma_struct,100);
      printf("\n");
    }
    int j=convertOctalToDecimal(atoi(ma_struct.size));
    //printf("size %d\n",j);
    lseek(fd,512*arrondi512(j),SEEK_CUR);
  }
  //close(fd2);
  close(fd);

  while((opt=getopt(argc,argv,optstring))!=EOF) {
    switch (opt){
      case 'x':
        printf("option x \n");
        int fd2 = open(argv[argc-1],O_RDONLY);
        while(read(fd2,&ma_struct,512)!=0) {
        /*  if(strcmp(ma_struct.name,"\0")!=0) {
            if ((suite=strrchr(ma_struct.name,'/'))!=NULL) {
              printf("%s\n",suite);
              if (strcmp(suite,"/\0")==0) {
                if(fork()==0) {
                  execl("/bin/mkdir","mkdir",ma_struct.name,NULL);
                  exit(0);
                }
              }
              else {
                if(fork()==0) {
                  execl("/bin/touch","touch",ma_struct.name,NULL);
                  exit(0);
                }
              }
            }
            else {
              if(fork()==0) {
                execl("/bin/touch","touch",ma_struct.name,NULL);
                exit(0);
              }
            } }
*/
           if (strcmp(ma_struct.typeflag,"5")==0){
              if(fork()==0) {
                execl("/bin/mkdir","mkdir",ma_struct.name,NULL);
                exit(0);
              }
            }
          }
        close(fd2);
        int fd3 = open(argv[argc-1],O_RDONLY);
        while(read(fd3,&ma_struct,512)!=0) {
          if(strcmp(ma_struct.typeflag,"0")==0) {
            if(fork()==0) {
              execl("/bin/touch","touch",ma_struct.name,NULL);
              exit(0);
            }
          }
          int j=convertOctalToDecimal(atoi(ma_struct.size));
          char buffer[j];
          int fdfichier = open(ma_struct.name,O_WRONLY);
          read(fd3,buffer,j);
          printf("%s\n",buffer);
          write(fdfichier,buffer,j);
          lseek(fd3,(512*arrondi512(j))-j,SEEK_CUR);
          close(fdfichier);
        }
        close(fd3);
      /*  int fd4 = open(argv[argc-1],O_RDONLY);
        while(read(fd3,&ma_struct,512)!=0) {
          if(strcmp(ma_struct.typeflag,"2")==0) {
            if(fork()==0) {
              execl("/bin/ln","ln -s",ma_struct.name,NULL);
              exit(0);
            }
          }
        }
        close(fd4);*/
        break;

      case 'l':
        printf("option l \n" );
        break;
      case 'p':
        printf("option p \n");
        break;
      case 'z':
        printf("option z \n");
        break;
    }
  }

  return 0;
}
