#define MAX_MTU 1280
#define MAX_ENTRIES 10

/* bitfields are underused these days - use 'em or nuke em */
/* FIXME, make a union */

struct query_options {
  int urls:1;
  int snippets:1;
  int ads:1;
  int titles:1;
  int misc:1;
  int reg:1;
  int reverse:1;
  int broadcast:1;
  int multicast:1;
  int force:1; 
  int secure:1;
  int xml:1;
  int html:1;
  int offline:1;
  int lucky:1;
  int prime:1;
  int cache:1;
  int engine:1;
  int mirror:1;
  int nresults;
  int position;
  char *engine_name;
  char *outputfile;
  char *language;
};

typedef struct query_options QueryOptions;

struct query_args {
  char *query;
  QueryOptions opt;        // 
  int entries;
  unsigned char qsha1[20]; // Query's Sha1 - text? prefer binary... later
  unsigned char asha1[20]; // Answer's Sha1
  char *links[MAX_ENTRIES]; 
  char *snippets[MAX_ENTRIES];
  char *titles[MAX_ENTRIES];
  char *ads[MAX_ENTRIES];
  char *misc[MAX_ENTRIES];
};

typedef struct query_args QueryData;

extern int query_main(QueryData *answers, char *host);
extern int answer_parse(QueryData *q);
