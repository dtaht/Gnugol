#ifndef _gnugol_maps
#define _gnugol_maps
#include <string.h>

const char * gnugol_nil = "";

struct gnugol_intmap {
	int id;
	char *s;
};

struct gnugol_charmap {
	char *id;
	char *s;
};

typedef struct gnugol_intmap gnugol_intmap_t;
typedef struct gnugol_charmap gnugol_charmap_t;

// Maybe NULL rather than nil?

inline int gnugol_get_intmap_id(const gnugol_intmap_t *map[], char *s) {
	for(int i = 0; map[i]->s != NULL; i++) {
		if(strcmp(map[i]->s,s) == 0)
			return(map[i]->id);
	}
	return(-1);
}

inline const char *gnugol_get_intmap_s(const gnugol_intmap_t *map[],
				       const int id) {
	for(int i = 0; map[i]->s != NULL; i++) {
		if(map[i]->id == id) return(map[i]->s);
	}
	return(gnugol_nil);
}

inline const char *gnugol_get_charmap_id(const gnugol_charmap_t *map[],
					const char *s) {
	for(int i = 0; map[i]->s != NULL; i++) {
		if(strcmp(map[i]->s,s) == 0) return(map[i]->id);
	}
	return(gnugol_nil);
}

inline const char *gnugol_get_charmap_s(const gnugol_charmap_t *map[],
					const char *id) {
	for(int i = 0; map[i]->s != NULL; i++) {
		if(strcmp(map[i]->id,id) == 0)
			return(map[i]->s);
	}
	return(gnugol_nil);
}

#endif
