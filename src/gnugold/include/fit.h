#ifndef gnugol_common_fit
#define gnugol_common_fit

int compress_results(QueryData *q, char *packet, int size)
// int compress_results(query *q)
int fit_results(query *q);
int fit_buffer(int results, int buffer_size, QueryData *q);
#endif
