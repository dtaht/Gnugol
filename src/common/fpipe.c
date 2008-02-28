#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "query.h"

#define PARENT_RDR 0
#define CHILD_RDR 0
#define CHILD_WTR 1
#define PARENT_WTR 1
int
main(int argc, char *argv[])
       {
           int pfd[2];
	   int cfd[2];
           pid_t cpid;
	   int i = 0;
	   FILE *fdr;
	   FILE *fdw;

	   char buf[MAX_ENTRIES][MAX_MTU];
	   char snip[MAX_ENTRIES][MAX_MTU];
	   char title[MAX_ENTRIES][MAX_MTU];
	   int blen[MAX_ENTRIES];
	   int slen[MAX_ENTRIES];
	   int tlen[MAX_ENTRIES];
           if (pipe(pfd) == -1) {
               perror("pipe");
               exit(EXIT_FAILURE);
           }
           if (pipe(cfd) == -1) {
               perror("pipe");
               exit(EXIT_FAILURE);
           }
           cpid = fork();
           if (cpid == -1) {
               perror("fork");
               exit(EXIT_FAILURE);
           }
           if (cpid == 0) {    /* Child reads from pipe */
		close(pfd[PARENT_RDR]);
		close(cfd[PARENT_WTR]);
          	dup2(pfd[CHILD_WTR],1);   /* make 1 same as write-to end of pipe  */
          	dup2(cfd[CHILD_RDR],0);   /* make 0 same as read-from end of pipe  */
          	//close(pfd[1]);    /* close excess fildes                  */
          	//close(pfd[0]);    /* close excess fildes                  */
		execl("/home/d/bin/gnugol2","gnugol2", NULL);
		// execl("gnugold","gnugold",NULL);
               _exit(EXIT_SUCCESS); // not reached
           } else {            /* Parent writes argv[1] to pipe */
		close(cfd[PARENT_RDR]);
		close(pfd[PARENT_WTR]);
          	dup2(cfd[CHILD_WTR],1);   /* make 1 same as write-to end of pipe  */
          	dup2(pfd[CHILD_RDR],0);   /* make 0 same as read-from end of pipe  */
		fdr = stdin; // fdopen(pfd[1],"r");
		fdw = stdout; // fdopen(pfd[0],"w");
		char *p = "this is a test\nso is this\n";
		fwrite(p,strlen(p),1,fdw);
		fflush(fdw);
		int n;
		while (( blen[i]=fgets(&buf[i],MAX_MTU,fdr) > 0) && i < MAX_ENTRIES) {
			printf("LINE: %s", buf[i]);
			i++;
			if(     strncmp("END",buf[i],3) == 0 || 
				strncmp("TLE",buf[i],3) == 0 ||
				strncmp("LNK",buf[i],3) == 0 ||
				strncmp("SNP",buf[i],3) == 0) {
				break;
				}
		}
		while (( blen[i]=fgets(&buf[i],MAX_MTU,fdr) > 0) && i < MAX_ENTRIES) { 
			if(!(   strncmp("END",buf[i],3) == 0 || 
				strncmp("TLE",buf[i],3) == 0 ||
				strncmp("LNK",buf[i],3) == 0 ||
				strncmp("SNP",buf[i],3) == 0)) {
				break;
				}
		}
		i=0;
		while (( slen[i]=fgets(&title[i],MAX_MTU,fdr) > 0) && i < 10) {
			printf("TLEL: %s", title[i]);
			if(     strncmp("END",title[i],3) == 0 || 
				strncmp("TLE",title[i],3) == 0 ||
				strncmp("LNK",title[i],3) == 0 ||
				strncmp("SNP",title[i],3) == 0) {
				break;
				}
			i++;
		}
		while (( blen[i]=fgets(&title[i],MAX_MTU,fdr) > 0) && i < MAX_ENTRIES) { 
			if(!(   strncmp("END",title[i],3) == 0 || 
				strncmp("TLE",title[i],3) == 0 ||
				strncmp("LNK",title[i],3) == 0 ||
				strncmp("SNP",title[i],3) == 0)) {
				break;
				}
		}
		i=0;
		while (( slen[i]=fgets(&snip[i],MAX_MTU,fdr) > 0) && i < 10) {
			printf("SNIP: %s", snip[i]);
			if(     strncmp("END",snip[i],3) == 0 || 
				strncmp("TLE",snip[i],3) == 0 ||
				strncmp("LNK",snip[i],3) == 0 ||
				strncmp("SNP",snip[i],3) == 0) {
				break;
				}
			i++;
		}
	
               close(pfd[0]);          /* Reader will see EOF */
               wait(NULL);             /* Wait for child */
               exit(EXIT_SUCCESS);
           }
       }

