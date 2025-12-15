#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char *argv[]) {
   pid_t val;
   if (argc==1) {
      if (execl("/bin/ls", "ls","-la",NULL)<0) {
      printf ("Message 1\n"); exit(1); }
    }
   else if (argc==2) {
           val= fork();
           if (execl("/bin/cat", "cat",argv[1],NULL)<0) {
               printf("Message 2\n"); exit(2);
            }
    }
   while (wait(NULL)!=-1) printf ("waiting \n");
   printf("Message 3\n");
   exit(0);
}