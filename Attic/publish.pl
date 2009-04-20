#!/usr/bin/perl

# Dependencies (external programs):
# - Java: <http://java.sun.com/javase/downloads/>
# - xmllint: <http://xmlsoft.org/xmldtd.html#validate1>
# - xmlto: <http://cyberelk.net/tim/software/xmlto/>


use strict;
use threads;
use utf8;
use warnings;

use Archive::Extract;
use Crypt::SSLeay;
use English;
use File::Basename;
use File::Path;
use File::Spec::Functions;
use LWP;
use XML::DOM;


eval { exit main(@ARGV) };
print "Error: $EVAL_ERROR\n";
exit 1;


sub download {
    my ($link, $output) = @ARG;
    my $browser = new LWP::UserAgent;
    my %options = ('User-Agent' => 'Mozilla');
    
    if (defined $output) {
        $options{':content_file'} = $output;
    }
    
    my $response = $browser->get($link, %options);
    die $response->status_line unless $response->is_success;
    
    return defined $output ? $output : $response->decoded_content;
}


sub get_msvs {
    my ($cache_root) = @ARG;
    my $cache = catdir($cache_root, 'MSVS');
    
    unless (-e $cache) {
        print "Downloading Multi-Schema Validator Schematron add-on...\n";
        
        my $base_url = 'https://msv.dev.java.net';
        my $list = "$base_url/servlets/ProjectDocumentList?folderID=101";
        my ($leaf_url) = (download($list) =~ m/"([^"]+relames[^"]+\.zip)"/);
        my $url = $base_url.$leaf_url;
        my $file = fileparse($url);
        
        mkpath($cache);
        
        my $path = download($url, catfile($cache, $file));
        my $archive = new Archive::Extract(archive => $path);
        
        $archive->extract(to => $cache) or die $archive->error;
        unlink $path;
    }
    
    return catfile(catdir($cache, ls($cache)), 'relames.jar');
}


sub get_rng {
    my ($cache_root) = @ARG;
    my $cache = catdir($cache_root, 'RNG');
    
    unless (-e $cache) {
        print "Downloading DocBook RELAX NG schema...\n";
        
        my $url = 'http://www.docbook.org/xml/5.0/rng/docbook.rng';
        my $file = fileparse($url);
        
        mkpath($cache);
        download($url, catfile($cache, $file));
    }
    
    return catfile($cache, ls($cache));
}


sub get_saxon {
    my ($cache_root) = @ARG;
    my $cache = catdir($cache_root, 'Saxon');
    
    unless (-e $cache) {
        print "Downloading Saxon XSLT processor...\n";
        
        my $list = 'http://saxon.sourceforge.net/';
        my ($url) = (download($list) =~ m/"([^"]+saxonb[^"]+j\.zip)"/);
        my $file = fileparse($url);
        
        mkpath($cache);
        
        my $path = download("$url?download", catfile($cache, $file));
        my $archive = new Archive::Extract(archive => $path);
        
        $archive->extract(to => $cache) or die $archive->error;
        unlink $path;
    }
    
    return catfile($cache, 'saxon9.jar');
}


sub get_xsl {
    my ($cache_root) = @ARG;
    my $cache = catdir($cache_root, 'XSL');
    
    unless (-e $cache) {
        print "Downloading DocBook XSL-NS style sheets...\n";
        
        my $base = 'http://docbook.sourceforge.net/release/xsl-ns';
        my $info = "$base/current/VERSION";
        my ($version) = (download("$base/current/VERSION") =~ m/<fm:Version>([^<]+)/);
        my $file = "docbook-xsl-ns-$version.tar.bz2";
        my $url = "http://prdownloads.sourceforge.net/docbook/$file?download";
        
        mkpath($cache);
        
        my $path = download($url, catfile($cache, $file));
        my $archive = new Archive::Extract(archive => $path);
        
        $archive->extract(to => $cache) or die $archive->error;
        unlink $path;
    }
    
    my $root = catdir($cache, ls($cache));
    return catfile(catdir($root, 'xhtml'), 'docbook.xsl');
}


sub ls {
    my ($path) = @ARG;
    
    opendir my ($directory), $path;
    my @files = grep {$ARG ne curdir and $ARG ne updir} readdir $directory;
    closedir $directory;
    
    return ((@files == 1) && !wantarray) ? pop @files : @files;
}


sub main {
    if (@ARG != 1) {
        print "Compiles documents in DocBook format to HTML.\n";
        print "Usage: <document file>\n";
        return 1;
    }
    
    my ($file) = @ARG;
    my $document = new XML::DOM::Parser->parsefile($file);
    my $version = $document->getDocumentElement()->getAttribute('version');
    my $publish = ($version =~ m/^5/) ? \&publish_v5 : \&publish;
    
    $document->dispose();
    my ($validate, $compile) = $publish->($file);
    
    print "Validating...\n";
    system $validate;
    
    print "Compiling...\n";
    system $compile;
    
    return 0;
}


sub publish {
    my ($file) = @ARG;
    my $validate = "xmllint --noout --valid \"$file\"";
    my $compile = "xmlto html-nochunks \"$file\"";
    
    return ($validate, $compile);
}


sub publish_v5 {
    my ($file) = @ARG;
    my $cache = catdir($ENV{USERPROFILE} || $ENV{HOME}, '.DocBook~');
    my $out = catfile(dirname($file), basename($file, '.xml').'.html');
    
    my %data = (
        msvs => \&get_msvs,
        rng => \&get_rng,
        saxon => \&get_saxon,
        xsl => \&get_xsl,
    );
    
    if ($OSNAME eq 'MSWin32') {
        $data{$ARG} = $data{$ARG}->($cache) for sort keys %data;
    }
    else {
        $data{$ARG} = threads->create($data{$ARG}, $cache) for sort keys %data;
        $data{$ARG} = $data{$ARG}->join() for sort keys %data;
    }
    
    defined $data{$ARG} or die "Download failed" for sort keys %data;
    
    my ($msvs, $rng, $saxon, $xsl) = @data{qw(msvs rng saxon xsl)};
    my $validate = "java -jar \"$msvs\" \"file://localhost/$rng\" \"$file\"";
    my $compile = "java -jar \"$saxon\" \"-s:$file\" \"-xsl:$xsl\" \"-o:$out\"";
    
    return ($validate, $compile);
}
