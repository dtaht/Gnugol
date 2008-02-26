#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>
#include <unistd.h>

#include "connect_client.h"
#include "port.h"

char links[10][1024];
char snippets[10][1024];

char answer[1280];
char *
query_main(int argc, char *query,char *host)
{
    int connfd, n, m;
    char *myhost;

    myhost = "localhost";
    if (argc == 2) 
        myhost=host;

    connfd = connect_client(myhost, QUERY_PORT, AF_UNSPEC, SOCK_DGRAM);

    if (connfd < 0) {
         fprintf(stderr,
                 "client error:: could not create connected socket "
                 "socket\n");
         return NULL;
    }

    m= write(connfd, query, strlen(query));

    memset(answer, 0, sizeof(answer));

    n = read(connfd,
             answer,
             sizeof(answer));

    close(connfd);
    char *s = answer;
    char buf[1024];
    int i = 0;
    int z = 0;
    int done = 0;
    memset(buf,0,sizeof(buf));
    // The less said about sscanf the better
    // FIXME - GOTOREGEX.
    if(strncmp("LNK",s,3)==0) {
      s = &s[4];
      while ((sscanf(s,"%s[^\n]",&buf)> 0) && !done) {
	if (strncmp("htt",buf,3)==0 || strncmp("ftp",buf,3)==0 || strncmp("gnugol",buf,6)==0) {
	  strcpy(links[i++],buf);
	  s = s + strlen(buf)+1;
	  memset(buf,0,sizeof(buf));
	} else { 
	  done = 1; 
	}
      }
    }
    done = 0; i=0; int t= 0;
    s = &s[4]; // END;
    printf("SNIPPED PROCESSING %s<br>",s);
    if(strncmp("SNP",s,3)==0) {
      s = &s[4];
      int done = 0;
      strcpy(snippets[0],"");
      // DAMN IT IT'S ALL WHITESPACE
      while ((t=sscanf(s,"%s[^\n]",&buf)> 0) && !done) {
	if (strncmp("END",buf,3)!=0) {
	  if(strcmp("\n",buf)==0) { 
	      i++;
	      s = s+1;
	      strcpy(snippets[i],"");
	    } else {
	      strcat(snippets[i],buf);
	      strcat(snippets[i]," ");
	      s = s + strlen(buf) +t;
	    }
	  printf("s is now: %s<br>",snippets[i]);
	  if(strncmp("\n",s,1) == 0) {
	    i++;
	    s = s+1;
	    strcpy(snippets[i],"");
	  }
	  
	} else { 
	  done = 1; 
	}
	memset(buf,0,sizeof(buf));
      }
    } else { 
      printf("terror: %s\n",s); 
    }

    for (z = 0; z <= i; z++) {
      //      printf("<a href=%s>%s</a><br>", links[z],snippets[z]); // snippet[z]
      printf("<a href=%s>%s</a><br>", links[z],snippets[z]); // snippet[z]
    }
    //    printf("link3: %s", links[2]);
    printf("Answer: %s", answer);
    return (char *) answer;
}
