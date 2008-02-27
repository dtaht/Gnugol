#define MAX_MTU 1280
#define MAX_ENTRIES 10

struct query_args {
  int sha1hash[4];
  char *links[MAX_ENTRIES];
  char *snippets[MAX_ENTRIES];
  char *titles[MAX_ENTRIES];
  char *ads[MAX_ENTRIES];
  char *misc[MAX_ENTRIES];
};

typedef struct query_args QueryData;

extern int answer_parse(char *s, QueryData *q);
