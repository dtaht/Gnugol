#ifndef gnugol_default_formats
#define gnugol_default_formats
/*    format = q->text | q->xml << 1 | q->html << 2 | 
      q->ssml << 3 | q->org << 4 | q->wiki << 5; */

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
#define FORMATDEFAULT FORMATORG

#endif
