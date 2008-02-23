#!/usr/bin/python

import sys
import string
import codecs
import google

print 'Content-type: text/htmln'

print '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"'
print ' "DTD/xhtml1-strict.dtd">'
print '<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">'
print '<head>'
print '    <meta http-equiv="Content-Type" content="text/html; CHARSET=UTF-8" />'
print '    <title>Google with Python</title>'
print '    <link rel="stylesheet" href="/style/python-google.css"'
print ' type="text/css" media="screen, handheld" />'
print '</head>'
print '<body>'

print '<h1>Google with Python</h1>'

google.LICENSE_KEY = 'gHbcd0RQFHKbR2PQ7o4q/LKfCMVB03Ru'
sys.stdout = codecs.lookup('utf-8')[-1](sys.stdout)

query = "ipv6 mobility"
data = google.doGoogleSearch(query)

print '<p><strong>1-10 of "' + query + '" total results for '
print str(data.meta.estimatedTotalResultsCount) + '</strong></p>'

for result in data.results:
    title = result.title
    title = title.replace('<b>', '<strong>')
    title = title.replace('</b>', '</strong>')

    snippet = result.snippet
    snippet = snippet.replace('<b>','<strong>')
    snippet = snippet.replace('</b>','</strong>')
    snippet = snippet.replace('<br>','<br />')

    print '<h2><a href="' + result.URL + '">' + title + '</a></h2>'
    print '<p>' + snippet + '</p>'

print '</body>'
print '</html>'
