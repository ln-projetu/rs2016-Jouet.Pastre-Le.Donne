//main argument fichier -> récuperer la structure POSIX header, puis lire cette structure, lister
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>

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

int main(int argc, char *argv[]) {
  int opt;
  char optstring[]="xlpz";
  //char test[512];
  int fd = open(argv[argc-1],O_RDONLY);
  //int fd2 = open("header.txt",O_WRONLY);
  struct header_posix_ustar ma_struct;
  while(read(fd,&ma_struct,512)!=0) {
    //read(fd,&ma_struct,512);
    //write(fd2,&ma_struct,512);
    write(1,&ma_struct,100);
    printf("\n");
  }

  //close(fd2);
  close(fd);

  while((opt=getopt(argc,argv,optstring))!=EOF) {
    switch (opt){
      case 'x':
        printf("option x \n");
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
