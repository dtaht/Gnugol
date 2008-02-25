/* Change this if the SERVER_NAME environment variable does not report
	the true name of your web server. */
#if 1
#define SERVER_NAME cgiServerName
#endif
#if 0
#define SERVER_NAME "www.boutell.com"
#endif

/* The format of the google query string is well known. The only required
field is the q.

http://www.google.com.au/search?q=gnogal+test&ie=utf-8&oe=utf-8&aq=t&rls=com.ubuntu:en-US:official&client=firefox-a

*/

/* You may need to change this, particularly under Windows;
	it is a reasonable guess as to an acceptable place to
	store a saved environment in order to test that feature. 
	If that feature is not important to you, you needn't
	concern yourself with this. */

#ifdef WIN32
#define SAVED_ENVIRONMENT "c:\\cgicsave.env"
#else
#define SAVED_ENVIRONMENT "/tmp/cgicsave.env"
#endif /* WIN32 */

#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

void HandleSubmit();
void ShowForm();
void CookieSet();
void Entries();
void Cookies();
void LoadEnvironment();
void SaveEnvironment();
extern char *query_main(int argc, char *q, char *host);
//void Query(char *query) { query_main(2,query,"toutatis.taht.net"); }
void Query(char *query) { query_main(2,query,"2001:4f8:3:36:2e0:81ff:fe23:90d3"); }

int cgiMain() {
#ifdef DEBUG
	LoadEnvironment();
#endif /* DEBUG */
	/* Load a previously saved CGI scenario if that button
		has been pressed. */
	if (cgiFormSubmitClicked("loadenvironment") == cgiFormSuccess) {
		LoadEnvironment();
	}
	/* Set any new cookie requested. Must be done *before*
		outputting the content type. */
	CookieSet();
	/* Send the content type, letting the browser know this is HTML */
	cgiHeaderContentType("text/html");
	/* Top of the page */
	fprintf(cgiOut, "<html><head>\n");
	fprintf(cgiOut, "<title>GnuGol</title></head>\n");
	fprintf(cgiOut, "<body><h1>GnuGol</h1>\n");
	/* If a submit button has already been clicked, act on the 
		submission of the form. */
	if ((cgiFormSubmitClicked("btnG") == cgiFormSuccess) ||
		cgiFormSubmitClicked("saveenvironment") == cgiFormSuccess)
	{
		HandleSubmit();
		fprintf(cgiOut, "<hr>\n");
	}
	/* Now show the form */
	ShowForm();
	/* Finish up the page */
	fprintf(cgiOut, "</body></html>\n");
	return 0;
}

void HandleSubmit()
{
  char query[1024];
  cgiFormStringNoNewlines("q", query, 1024);
  fprintf(cgiOut, "Name: ");
  cgiHtmlEscape(query);
  fprintf(cgiOut, "<BR>\n");
  
  Query(query);
  //Cookies();
  /* The saveenvironment button, in addition to submitting the form,
     also saves the resulting CGI scenario to disk for later
     replay with the 'load saved environment' button. */
  if (cgiFormSubmitClicked("saveenvironment") == cgiFormSuccess) {
    SaveEnvironment();
  }
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
	
void ShowForm()
{
	fprintf(cgiOut, "<form method=\"GET\"");
	fprintf(cgiOut, "action=\"");
	cgiValueEscape(cgiScriptName);
	fprintf(cgiOut, "\"><p>");
	fprintf(cgiOut, "Prefetch");
	fprintf(cgiOut, "<input type=\"checkbox\" name=\"prefetch\" checked>");
	fprintf(cgiOut, "Query: <input type=\"text\" name=\"q\">Query\n");
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

