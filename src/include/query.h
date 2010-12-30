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
  int offline:1;
  int page:1;
  int prime:1;
  int cache:1;
  int engine:1;
  int mirror:1;
  int input:1;
  int plugin:1;
  int output:1;
  int blind:1;
  int verbose:1;
  int ipv4:1;
  int ipv6:1;
  int trust:1;
  int dontfork:1;
  int footer:1;
  int header:1;
  int about:1;
  int url_escape:1;
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
