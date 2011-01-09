#ifndef _gnugol_query_h

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#define MAX_MTU 1280
#define MAX_ENTRIES 8

enum gnugol_formatter {
  FORMATNONE,
  FORMATRAW,
  FORMATTERM,
  FORMATTEXT,
  FORMATHTML,
  FORMATHTML5,
  FORMATELINKS,
  FORMATWIKI,
  FORMATIKI,
  FORMATORG,
  FORMATSSML,
  FORMATJSON,
  FORMATXML,
  FORMATMDWN,
  FORMATCSV,
  FORMATSQL,
  FORMATTEXTILE,
  FORMATMAN,
  FORMATDNS,
  FORMATINFO,
  FORMATLISP
};


struct gnugol_buffer_obj {
  int size;
  int len;
  char *s;
};

typedef struct gnugol_buffer_obj buffer_obj_t;

/* bitfields are underused these days - use 'em or nuke em */

struct query_options {
  unsigned int urls:1;
  unsigned int ads:1;
  unsigned int misc:1;
  unsigned int reg:1;
  unsigned int reverse:1;
  unsigned int broadcast:1;
  unsigned int multicast:1;
  unsigned int force:1;
  unsigned int secure:1;
  unsigned int offline:1;
  unsigned int page:1;
  unsigned int prime:1;
  unsigned int cache:1;
  unsigned int engine:1;
  unsigned int mirror:1;
  unsigned int input:1;
  unsigned int plugin:1;
  unsigned int output:1;
  unsigned int blind:1;
  unsigned int verbose:1;
  unsigned int ipv4:1;
  unsigned int ipv6:1;
  unsigned int trust:1;
  unsigned int dontfork:1;
  unsigned int footer:1;
  unsigned int header:1;
  unsigned int about:1;
  unsigned int url_escape:1;
  unsigned int break_words:1;
  int snippets;
  int titles;
  int desc;
  int indent;
  int debug;
  int nresults;
  int position;
  int safe; // 0, 1, 2
  enum gnugol_formatter format;
  int returned_results;
  char *header_str;
  char *footer_str;
  char *license_str;
  char *engine_name;
  char *output_file;
  char *input_file;
  char *plugin_file;
  char *server;
  char *client;
  buffer_obj_t out;
  buffer_obj_t err;
  buffer_obj_t wrn;
  char input_language[12];
  char output_language[12];
  char querystr[2048];
  char keywords[1024];
};

typedef struct query_options QueryOptions_t;
#endif
