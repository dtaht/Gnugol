#ifndef gnugol_plugin
#ifdef DUMMY_SERVER
#else
extern int gnugol_plugin_gscrape_init();
extern int gnugol_plugin_gscrape(QueryData *q);
#endif
#endif
