@echo off
set mail=%TEMP%\..\Application Data\Opera\Opera\mail\index.ini
"Opera OPML.pl" "%mail%" > opera-newsfeeds.opml
