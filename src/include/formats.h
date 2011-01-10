#ifndef gnugol_default_formats
#define gnugol_default_formats

#define GNUGOL_SITE "http://gnugol.taht.net/"
#define FORMATDEFAULT FORMATORG

#define BUFFER_SIZE  (64 * 1024)  /* 64 KB */
#define URL_SIZE     (2048)
#ifndef PATH_MAX
#define PATH_MAX (1024)
#endif
#define SNIPPETSIZE (2048)
#define OUTPUTSIZE (64*1024)

#define STRIPHTML(a) strip_html(SNIPPETSIZE,a)
#define NULLP(str) (str == NULL ? "" : str)

// TROUBLESOME Shorthand macros to get buffer management right
#ifndef DEBUG
#define GNUGOL_OUTF(QUERYOBJARG ,...) do { QUERYOBJARG->out.len += snprintf(&(QUERYOBJARG->out.s[QUERYOBJARG->out.len]), QUERYOBJARG->out.size-QUERYOBJARG->out.len, __VA_ARGS__); } while (0)
#define GNUGOL_OUTE(QUERYOBJARG ,...) do { QUERYOBJARG->err.len += snprintf(&(QUERYOBJARG->err.s[QUERYOBJARG->err.len]), QUERYOBJARG->err.size-QUERYOBJARG->err.len, __VA_ARGS__); } while (0)
#define GNUGOL_OUTW(QUERYOBJARG ,...) do { QUERYOBJARG->wrn.len += snprintf(&(QUERYOBJARG->wrn.s[QUERYOBJARG->wrn.len]), QUERYOBJARG->wrn.size-QUERYOBJARG->wrn.len, __VA_ARGS__); } while (0)
#else
#define GNUGOL_OUTF(QUERYOBJARG ,...) do { QUERYOBJARG->out.len += snprintf(&(QUERYOBJARG->out.s[QUERYOBJARG->out.len]), QUERYOBJARG->out.size-QUERYOBJARG->out.len, __VA_ARGS__); \
    if (QUERYOBJARG->debug > 5) fprintf(stderr,  __VA_ARGS__);  } while (0)
#define GNUGOL_OUTE(QUERYOBJARG ,...) do { QUERYOBJARG->err.len += snprintf(&(QUERYOBJARG->err.s[QUERYOBJARG->err.len]), QUERYOBJARG->err.size-QUERYOBJARG->err.len, __VA_ARGS__); \
    if (QUERYOBJARG->debug > 1) fprintf(stderr,  __VA_ARGS__);  } while (0)
#define GNUGOL_OUTW(QUERYOBJARG ,...) do { QUERYOBJARG->wrn.len += snprintf(&(QUERYOBJARG->wrn.s[QUERYOBJARG->wrn.len]), QUERYOBJARG->wrn.size-QUERYOBJARG->wrn.len, __VA_ARGS__); \
    if (QUERYOBJARG->debug > 2) fprintf(stderr,  __VA_ARGS__);  } while (0)
#endif

extern int gnugol_init_QueryOptions(QueryOptions_t *q);
extern int gnugol_reset_QueryOptions(QueryOptions_t *q);
extern int gnugol_free_QueryOptions(QueryOptions_t *q);
extern int gnugol_header_out(QueryOptions_t *q);
extern int gnugol_keywords_out(QueryOptions_t *q);
extern int gnugol_footer_out(QueryOptions_t *q);
extern int gnugol_result_out(QueryOptions_t *q, const char *url, const char *title, const char *snippet);
extern int gnugol_result_out_long(QueryOptions_t *,const char *,const char *,const char *,const char *);
#endif
