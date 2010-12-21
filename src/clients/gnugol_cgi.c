#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

#include "query.h"

void HandleSubmit();
void HandleConfig(QueryOptions *o);
void ShowForm();
void CookieSet();
void Entries();
void Cookies();

#define SERVER_NAME "g.gnugol.org"
#define SAVED_ENVIRONMENT "/tmp/gnugol.env"

/* It will be simpler to just generate a cookie based on the set options */

int Query(QueryOptions *q) { 
  int i;
  int n = query_main(q,"::1");
  // int n = query_main(q,"2001:4f8:3:36:2e0:81ff:fe23:90d3");
  int output = 
    q->options.urls >> 3 | q->options.snippets >> 2 | 
    q->options.titles >> 1; // | q->options.ads ;
  //  fprintf(cgiOut,"shift = %d\n",output);

  for (i = 0; i < n; i++) {
    switch(output) {
    case 0: break;
    case 1: break;
    case 2: break;
    case 3: break;
    case 4: break;
    case 5: break;
    case 6: break;
    case 7: fprintf(cgiOut,"<a href=\"%s\">%s</a><br>",q->links[i],q->snippets[i]);
      break;
    case 8: break;
    case 9: break;
    case 10: break;
    case 11: break;
    case 12: fprintf(cgiOut,"<a href=\"%s\">%s</a><br>",q->links[i],q->snippets[i]);
      break;
    default: break;
    }
    // ignore all that for now
    fprintf(cgiOut,"<a href=\"%s\">%s</a><br>",q->links[i],q->snippets[i]);
  }
  return (n);
}

int Config(QueryData *q) {
      // Fixme, only allow trusted hosts to run the configuration
      // loadConfig(whatever); FIXME FAKE IT FOR NOW
  q->options.urls = 1;
  q->options.titles= 0;
  q->options.snippets= 1;
  q->options.nresults=3;
  q->options.position=0;
  titlepage(q,"Gnugol config");
  fprintf(cgiOut,"<form action=/cgi-bin/gnugol.cgi?config method=post>");
  HandleConfig(&q->options);
  fprintf(cgiOut,"<input type=submit name=reconfig>");
  fprintf(cgiOut,"</form>");
  fprintf(cgiOut, "<hr>\n");
}

int metadata(QueryData *q) {
  // Style sheets & Javascript
  return 0;
}

int titlepage(QueryData *q, char *title) {
  fprintf(cgiOut, "<html><head>\n");
  fprintf(cgiOut, "<title>%s</title>",title);
  metadata(q);
  fprintf(cgiOut,"</head>\n");
  if(!q->options.blind) {
    fprintf(cgiOut, "<body><span style=\"font-size:.8em; text-align:top\">");
    if(q->options.ipv6) {
      fprintf(cgiOut, "<a href=http://ipv6.google.com/images>Images</a> ");
      fprintf(cgiOut, "<a href=http://ipv6.google.com/news>News</a> ");
      fprintf(cgiOut, "<a href=http://ipv6.google.com/maps>Maps</a> ");
    } else {
      fprintf(cgiOut, "<a href=http://www.google.com/images>Images</a> ");
      fprintf(cgiOut, "<a href=http://www.google.com/news>News</a> ");
    }
    fprintf(cgiOut,"<span style=\"text-align:left\"> <a href=gnugol://gnugol>Way Faster Search</a> ");
    fprintf(cgiOut,"<a href=gnugol://IpV6>Ipv6 Enabled</a></span></span><br>\n");
    fprintf(cgiOut,"<img src=/gnugol/images/gnugol.png><br><hr>");
  } else {
    fprintf(cgiOut, "<body>");
  } 
}

int EmptySubmit(QueryData *q) {
  titlepage(q,"Gnugol Search");
  ShowForm(q);
  // FIXME - check to see if we've primed the cache recently
  // q->options.prime=1;
  // int n = query_main(q,"::1"); 
}

void HandleSubmit(QueryData *q)
{
  char query[1280];

  strcpy(query,q->keywords);
  //  cgiFormStringNoNewlines("q", query, 1024);
  cgiHtmlEscape(query);
  cgiFormString("q", query, sizeof(query));	
  titlepage(q,query);
  q->options.urls = q->options.snippets = 1;
  q->options.nresults = 3;
  Query(q);
  //  if (cgiFormSubmitClicked("saveenvironment") == cgiFormSuccess) {
  //  SaveEnvironment();
  // }
  fprintf(cgiOut,"<hr>");
  ShowForm(q);
}

// cgiFormStringNoNewlines("q",query->keywords,MAX_MTU);

#ifdef penabled
#undef penabled
#endif

#define penabled(a,b)  q->options.b = (cgiFormCheckboxSingle(a) == cgiFormSuccess) ? 0:1

int setup_options(QueryData *q) {
}

int cgiMain() {
  QueryData *q = calloc(sizeof(QueryData),1);
  CookieSet();
  /* Send the content type, letting the browser know this is HTML */
  cgiHeaderContentType("text/html");
  if (cgiFormSubmitClicked("config") == cgiFormSuccess)
    {
      q->options.blind = cgiFormCheckboxSingle("blind") == cgiFormSuccess ? 0:1;
      Config(q);
    } else {
    if (cgiFormSubmitClicked("btnG") == cgiFormSuccess) {
      q->options.blind = cgiFormCheckboxSingle("blind") == cgiFormSuccess ? 0:1;
      HandleSubmit(q);
    } else {
      q->options.blind = cgiFormCheckboxSingle("blind") == cgiFormSuccess ? 0:1;
      EmptySubmit(q);
    }
  }
  fprintf(cgiOut, "</body></html>\n");
  free(q);
  return 0;
}


// #define penabled(a,b)  fprintf(cgiOut,"<input type=checkbox name=\"" a "\" %s%s<br>", o->b ? "checked>" : ">",_(a));
#ifdef penabled
#undef penabled
#endif
#define penabled(a,b)  fprintf(cgiOut,"<input type=checkbox name=\"" a "\" %s%s<br>", o->b ? "checked>" : ">",a);
// (cgiFormCheckboxSingle("hungry") == cgiFormSuccess)

void HandleConfig(QueryOptions *o)
{
  fprintf(cgiOut,"You are requesting <input type=text name=results size=1 value=%d> results starting at position <input type=text name=pos size=3 value=%d><br>",o->nresults,o->position);
  penabled("urls",urls);
  penabled("titles",titles);
  penabled("snippets",snippets);
  penabled("ads",ads);
  penabled("misc",misc);
  penabled("reverse",reverse);
  penabled("broadcast",broadcast);
  penabled("multicast",multicast);
  penabled("force",force);
  penabled("cache",cache);
  penabled("xml",xml);
  penabled("html",html);
  penabled("offline",offline);
  penabled("lucky",lucky);
  penabled("register",reg);
  penabled("prime",prime);
  penabled("engine",engine);
  penabled("mirroring",mirror);
  penabled("plugin",plugin);
  penabled("blind",blind);
  penabled("ipv4",ipv4);
  penabled("ipv6",ipv6);
  penabled("dummy",dummy);
  penabled("debug",debug);
  penabled("trust",trust);
}

void Entries()
{
	char **array, **arrayStep;
	fprintf(cgiOut, "List of All Submitted Form Field Names:<p>\n");
	if (cgiFormEntries(&array) != cgiFormSuccess) {
		return;
	}
	arrayStep = array;
	fprintf(cgiOut, "<ul>\n");
	while (*arrayStep) {
		fprintf(cgiOut, "<li>");
		cgiHtmlEscape(*arrayStep);
		fprintf(cgiOut, "\n");
		arrayStep++;
	}
	fprintf(cgiOut, "</ul>\n");
	cgiStringArrayFree(array);
}

void Cookies()
{
	char **array, **arrayStep;
	char cname[1024], cvalue[1024];
	fprintf(cgiOut, "Cookies Submitted On This Call, With Values (Many Browsers NEVER Submit Cookies):<p>\n");
	if (cgiCookies(&array) != cgiFormSuccess) {
		return;
	}
	arrayStep = array;
	fprintf(cgiOut, "<table border=1>\n");
	fprintf(cgiOut, "<tr><th>Cookie<th>Value</tr>\n");
	while (*arrayStep) {
		char value[1024];
		fprintf(cgiOut, "<tr>");
		fprintf(cgiOut, "<td>");
		cgiHtmlEscape(*arrayStep);
		fprintf(cgiOut, "<td>");
		cgiCookieString(*arrayStep, value, sizeof(value));
		cgiHtmlEscape(value);
		fprintf(cgiOut, "\n");
		arrayStep++;
	}
	fprintf(cgiOut, "</table>\n");
	cgiFormString("cname", cname, sizeof(cname));	
	cgiFormString("cvalue", cvalue, sizeof(cvalue));	
	if (strlen(cname)) {
		fprintf(cgiOut, "New Cookie Set On This Call:<p>\n");
		fprintf(cgiOut, "Name: ");	
		cgiHtmlEscape(cname);
		fprintf(cgiOut, "Value: ");	
		cgiHtmlEscape(cvalue);
		fprintf(cgiOut, "<p>\n");
		fprintf(cgiOut, "If your browser accepts cookies (many do not), this new cookie should appear in the above list the next time the form is submitted.<p>\n"); 
	}
	cgiStringArrayFree(array);
}
	
void ShowForm(QueryData *q)
{
  char keywords[1080];
  fprintf(cgiOut, "<form method=\"GET\"");
  fprintf(cgiOut, "action=\"");
  cgiValueEscape(cgiScriptName);
  fprintf(cgiOut, "\"><p>");
  if(cgiFormString("q", keywords, sizeof(keywords)));	

  if(keywords[0] != '\0') {
    strcpy(q->keywords,keywords);
    //		cgiValueEscape(value); ?
    fprintf(cgiOut, "Query: <input type=\"text\" name=\"q\" value=\"");
    cgiValueEscape(q->keywords);
    fprintf(cgiOut,"\" >\n");
  } else {
    fprintf(cgiOut, "Query: <input type=\"text\" name=\"q\" >\n");
  }
  fprintf(cgiOut, "<input type=submit name=\"btnG\" value=\"Search\">");
  fprintf(cgiOut, "</form>\n");
}

void CookieSet()
{
	char cname[1024];
	char cvalue[1024];
	/* Must set cookies BEFORE calling cgiHeaderContentType */
	cgiFormString("cname", cname, sizeof(cname));	
	cgiFormString("cvalue", cvalue, sizeof(cvalue));	
	if (strlen(cname)) {
		/* Cookie lives for one day (or until browser chooses
			to get rid of it, which may be immediately),
			and applies only to this script on this site. */	
		cgiHeaderCookieSetString(cname, cvalue,
			86400, cgiScriptName, SERVER_NAME);
	}
}

void LoadEnvironment()
{
	if (cgiReadEnvironment(SAVED_ENVIRONMENT) != 
		cgiEnvironmentSuccess) 
	{
		cgiHeaderContentType("text/html");
		fprintf(cgiOut, "<head>Error</head>\n");
		fprintf(cgiOut, "<body><h1>Error</h1>\n");
		fprintf(cgiOut, "cgiReadEnvironment failed. Most "
			"likely you have not saved an environment "
			"yet.\n");
		exit(0);
	}
	/* OK, return now and show the results of the saved environment */
}

void SaveEnvironment()
{
	if (cgiWriteEnvironment(SAVED_ENVIRONMENT) != 
		cgiEnvironmentSuccess) 
	{
		fprintf(cgiOut, "<p>cgiWriteEnvironment failed. Most "
			"likely %s is not a valid path or is not "
			"writable by the user that the CGI program "
			"is running as.<p>\n", SAVED_ENVIRONMENT);
	} else {
		fprintf(cgiOut, "<p>Environment saved. Click this button "
			"to restore it, playing back exactly the same "
			"scenario: "
			"<form method=POST action=\"");
		cgiValueEscape(cgiScriptName);
		fprintf(cgiOut, "\">" 
			"<input type=\"submit\" "
			"value=\"Load Environment\" "
			"name=\"loadenvironment\"></form><p>\n");
	}
}

