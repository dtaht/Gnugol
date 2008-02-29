/* Fit the results into a buffer of size X */

int
compress_results(QueryData *q, buffer *packet, int size) {
	int i;
	int nsize = 0;
	nsize += ?q->options.url : 0, q->nurls;
	nsize += ?q->options.snippets : 0, q->nsnippets;
	nsize += ?q->options.titles : 0, q->ntitles;
	nsize += ?q->options.nads : 0, q->nads;
	nsize += ?q->options.misc : 0, q->nmisc;

	if(nsize > 4*size) { 
		return(-1); // odds are really good that we can't compress that many results, don't try
	}
	char *b = (char *) calloc (q->size + 20,1);

	// yea, yea, strcat's inefficient
	
	if(q->options.url) {
		for(i = 0; i < q->nurls; i++) {
			strcat(b,q->urls[i]);	
		}
	if(q->options.snippets) {
		for(i = 0; i < q->nsnippets; i++) {
			strcat(b,q->snippets[i]);	
		}
	if(q->options.titles) {
		for(i = 0; i < q->ntitles; i++) {
			strcat(b,q->titles[i]);	
		}
	if(q->options.ads) {
		for(i = 0; i < q->nads; i++) {
			strcat(b,q->ads[i]);	
		}
	if(q->options.misc) {
		for(i = 0; i < q->nmisc; i++) {
			strcat(b,q->misc[i]);	
		}
	// sha1 hash
	// compress_buffer
}
