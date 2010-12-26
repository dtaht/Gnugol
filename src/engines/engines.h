#ifndef gnugol_defined_engines
#define gnugol_defined_engines 1

extern int GNUGOL_DECLARE_ENGINE(search,google) (QueryOptions_t *q);
extern int GNUGOL_DECLARE_ENGINE(search,bing)   (QueryOptions_t *q);
extern int GNUGOL_DECLARE_ENGINE(search,wikipedia)(QueryOptions_t *q);
extern int GNUGOL_DECLARE_ENGINE(search,dummy)  (QueryOptions_t *q);

#endif
