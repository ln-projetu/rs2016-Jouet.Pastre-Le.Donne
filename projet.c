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
        //printf("option x \n");
        int fdd = open(argv[argc-1],O_RDONLY);
        while(read(fdd,&ma_struct,512)!=0) {
          int m=convertOctalToDecimal(atoi(ma_struct.mode));
           if (strcmp(ma_struct.typeflag,"5")==0){
              mkdir(ma_struct.name,ma_struct.mode);
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

        break;

      case 'l':
        //printf("option l \n" );
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
