// ==UserScript==
// @include http://www.mytvshows.org/mark/all/seen/*
// ==/UserScript==


var showName = location.href.match(/seen\/([^\/]+)/).pop();
location = 'http://www.mytvshows.org/show/' + showName + '/';
