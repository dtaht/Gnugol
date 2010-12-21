#ifndef gnugol_default_formats
#define gnugol_default_formats
// <a href=url>title</a> content
// url title content
#define FORMATNONE (0)
#define FORMATTERM (1)
#define FORMATHTML (2)
// Tiny web page of results
#define FORMATELINKS (3) 
// [[Title|url]] content
#define FORMATWIKI (4)
// * [[Title][url]] content
#define FORMATORG  (5)
// SSML Support
#define FORMATSSML (6)
#define FORMATJSON (7) 
#define FORMATXML (8) 
#define FORMATMDWN (9) 
#define FORMATCSV (10) 
#define FORMATSQL (11) 
#define FORMATDEFAULT FORMATORG

#define BUFFER_SIZE  (64 * 1024)  /* 64 KB */
#define URL_SIZE     (2048)
#define PATH_MAX (1024)
#define SNIPPETSIZE (2048) 
#define OUTPUTSIZE (64*1024)

#define STRIPHTML(a) strip_html(SNIPPETSIZE,a)

// Shorthand macros to get buffer management right

#define GNUGOL_OUTF(q ,...) do { q->out.len += snprintf(&(q->out.s[q->out.len]), q->out.size-q->out.len, __VA_ARGS__); } while (0)
#define GNUGOL_OUTE(q ,...) do { q->err.len += snprintf(&(q->err.s[q->err.len]), q->err.size-q->err.len, __VA_ARGS__); } while (0)
#define GNUGOL_OUTW(q ,...) do { q->wrn.len += snprintf(&(q->wrn.s[q->wrn.len]), q->wrn.size-q->wrn.len, __VA_ARGS__); } while (0)

extern int gnugol_init_QueryOptions(QueryOptions_t *q);
extern int gnugol_free_QueryOptions(QueryOptions_t *q);
extern int gnugol_header_out(QueryOptions_t *q);
extern int gnugol_keywords_out(QueryOptions_t *q);
extern int gnugol_footer_out(QueryOptions_t *q);
extern int gnugol_result_out(QueryOptions_t *q, const char *url, const char *title, const char *snippet, const char *ad);

#endif
