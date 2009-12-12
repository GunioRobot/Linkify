@echo off
set mail=%TEMP%\..\Application Data\Opera\Opera\mail
"Opera OPML.pl" "%mail%" > opera-newsfeeds.opml
