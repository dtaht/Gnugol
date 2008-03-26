#ifndef query_port_no_h

const char *QUERY_PORT = "3301";
const char *ANSWER_PORT = "3301";

// temporary addresses for SSM multicast hopefully unused
// It makes more sense to use more port numbers

const char *GNUGOL_IPV6_MCAST_SEND = "FF3F:BEEF::1";
const char *GNUGOL_IPV6_SMCAST_SEND = "FF3F:BEEF::2";
const char *GNUGOL_IPV6_SMAUTH_SEND = "FF3F:BEEF::3";

//const char *GNUGOL_IPV6_MCAST_RECEIVE = "FF3F:BEEF::3";
//const char *GNUGOL_IPV6_SMCAST_RECEIVE = "FF3F:BEEF::4";
//const char *GNUGOL_IPV6_SMAUTH_RECEIVE = "FF3F:BEEF::5";
#endif
