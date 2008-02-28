/* program sends user enter message from parent to child and back */
/* contributed by david rowe. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define EXECV
int gnugol_plugin_gscrape(argc, argv)
     int argc;
     char *argv[];
{
    int i, pid, p2c[2], c2p[2];
    char send[80], receive[80], *gets();
 
    if ( pipe(p2c) != 0) {
	printf("p2c pipe creation error");
	exit(1);
    }

    if ( pipe(c2p) != 0) {
	printf("c2p pipe creation error");
	exit(1);
    }

    i = 0;
    pid = fork();
    if (pid == -1) {
	printf("fork error\n");
	exit(0);
    }
 
    if (pid == 0) {                        /* CHILD process */
	dup2(c2p[1], 1); // stdout (e.g. printfs) get send to c2p[1]
	dup2(p2c[0], 0); // stdin comes from p2c[0]

#ifdef EXECV
        execv("inout",NULL);
#else	      
	while(1) {
        fgets(receive,80,stdin); 
	    fprintf(stderr, "Child received the following message: %s",receive);
	    printf("Really?\n");
	    fflush(stdout);
	}
#endif
	/* never gets here */
        exit(0);
	      
    }
    else {                                 /* PARENT process */
	sprintf(send, "I am your father\n");
        write(p2c[1],send,strlen(send));
	i = read(c2p[0],receive,80);       /* get message from child */
        receive[i] = '\0';
        fprintf(stderr,"Parent received: %s",receive);

	sprintf(send, "No, I can't back that up.\n");
        write(p2c[1],send,strlen(send));
        i = read(c2p[0],receive,80);       /* get message from child */
        receive[i] = '\0';
        fprintf(stderr,"Parent received: %s",receive);
    }
 
    return 0;
} 
