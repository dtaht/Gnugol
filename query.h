#define MAX_MTU 1280
#define MAX_ENTRIES 10

struct query_args {
  char *links[MAX_ENTRIES];
  char *snippets[MAX_ENTRIES];
  char *titles[MAX_ENTRIES];
};

extern int answer_parse(char *s, char *links[], char *snippets[]);
