#!/usr/bin/env perl

# External programs:
# - Java: <http://java.sun.com/javase/downloads/>
# - xmllint: <http://xmlsoft.org/xmldtd.html#validate1>
# - xmlto: <http://cyberelk.net/tim/software/xmlto/>

use defaults;
use Archive::Extract ();
use Crypt::SSLeay ();
use Path::Class ();
use URI ();
use WWW::Mechanize ();
use XML::DOM::XPath ();


sub detect_version {
    my ($file) = @ARG;
    my $doc = eval {XML::DOM::Parser->new->parsefile($file)};
    
    const my $PUB_ID_VERSION = qr{
        -//OASIS//DTD \s+ DocBook \s+ XML \s+ V ([\d.]+)//EN
    }x;
    
    if (defined $doc) {
        if ((my $version = $doc->findvalue('/*/@version')) ne '') {
            return $version;
        }
        elsif (defined(my $doctype = $doc->getDoctype)) {
            return $1 if $doctype->getPubId =~ m/^$PUB_ID_VERSION$/;
        }
    }
    else {
        my $xml = $file->slurp;
        
        if ($xml =~ m/< [^<>?]* \b version \s* = \s* " ([^"]+) "/gx) {
            return $1;
        }
        elsif ($xml =~ m/"$PUB_ID_VERSION"/) {
            return $1;
        }
    }
    
    return;
}


sub get_msvs {
    my ($cache_root) = @ARG;
    my $cache = $cache_root->subdir('MSVS');
    
    unless (-e $cache) {
        print "Downloading Multi-Schema Validator Schematron add-on...\n";
        
        my $mechanize = WWW::Mechanize->new;
        $mechanize->get('http://java.net/downloads/msv/nightly/');
        
        my $link = $mechanize->find_link(text_regex => qr/\b relames \b/x);
        my $file = $cache->file([$link->URI->path_segments]->[-1]);
        
        $cache->mkpath;
        $mechanize->get($link, ':content_file' => $file->stringify);
        
        my $archive = Archive::Extract->new(archive => $file);
        $archive->extract(to => $cache) or die $archive->error;
        $file->remove;
    }
    
    return [$cache->children]->[0]->file('relames.jar');
}


sub get_rng {
    my ($cache_root) = @ARG;
    my $cache = $cache_root->subdir('RNG');
    
    unless (-e $cache) {
        print "Downloading DocBook RELAX NG schema...\n";
        my $url = URI->new('http://www.docbook.org/xml/5.0/rng/docbook.rng');
        my $file = $cache->file([$url->path_segments]->[-1]);
        
        $cache->mkpath;
        WWW::Mechanize->new->get($url, ':content_file' => $file->stringify);
    }
    
    return [$cache->children]->[0];
}


sub get_saxon {
    my ($cache_root) = @ARG;
    my $cache = $cache_root->subdir('Saxon');
    
    unless (-e $cache) {
        print "Downloading Saxon XSLT processor...\n";
        
        my $mechanize = WWW::Mechanize->new;
        $mechanize->get('http://saxon.sourceforge.net/');
        
        my $link = $mechanize->find_link(url_regex => qr/\b saxonhe .+ j\.zip/x);
        my $file = $cache->file([$link->URI->path_segments]->[-2]);
        my $file_url = 'http://prdownloads.sourceforge.net/saxon/%s?download';
        
        $cache->mkpath;
        $mechanize->get(sprintf($file_url, $file->basename),
            ':content_file' => $file->stringify);
        
        my $archive = Archive::Extract->new(archive => $file);
        $archive->extract(to => $cache) or die $archive->error;
        $file->remove;
    }
    
    return [$cache->children]->[0];
}


sub get_xsl {
    my ($cache_root) = @ARG;
    my $cache = $cache_root->subdir('XSL');
    
    unless (-e $cache) {
        print "Downloading DocBook XSL-NS style sheets...\n";
        
        my $version = XML::DOM::Parser->new->parsefile(
            'http://docbook.sourceforge.net/release/xsl-ns/current/VERSION');
        my $file = $cache->file(sprintf 'docbook-xsl-ns-%s.tar.bz2',
            $version->findvalue('//fm:Version'));
        my $file_url = 'http://prdownloads.sourceforge.net/docbook/%s?download';
        
        $cache->mkpath;
        WWW::Mechanize->new->get(sprintf($file_url, $file->basename),
            ':content_file' => $file->stringify);
        
        my $archive = Archive::Extract->new(archive => $file);
        $archive->extract(to => $cache) or die $archive->error;
        $file->remove;
    }
    
    return [$cache->children]->[0]->subdir('xhtml')->file('docbook.xsl');
}


sub main {
    my ($file) = @ARG;
    my $version;
    
    if (defined $file) {
        $file = Path::Class::file($file);
        $version = detect_version($file);
    }
    else {
        foreach my $xml_file (grep m/\.xml$/i, Path::Class::dir->children) {
            $version = detect_version($xml_file);
            
            if (defined $version) {
                $file = $xml_file;
                print "Automatic detection of DocBook v$version: $file\n";
                last;
            }
        }
    }
    
    if (defined($file) && defined($version)) {
        publish($file, $version);
    }
    else {
        print << 'USAGE';
Compiles documents in DocBook format to HTML.

Options: [file]
USAGE
    }
    
    return;
}


sub publish {
    my ($file, $version) = @ARG;
    my $publish = sprintf 'publish_v%u', $version;
    my ($validate, $compile) = __PACKAGE__->can($publish)->($file);
    
    system @$validate;
    system @$compile;
    
    return;
}


sub publish_v4 {
    my ($file) = @ARG;
    my $validate = [qw(xmllint --noout --valid), $file];
    my $compile = [qw(xmlto html-nochunks), $file];
    
    return ($validate, $compile);
}


sub publish_v5 {
    my ($file) = @ARG;
    my $cache = Path::Class::dir($ENV{USERPROFILE} // $ENV{HOME}, '.DocBook~');
    my ($msvs, $rng, $saxon, $xsl) = map {$ARG->($cache)}
        \&get_msvs, \&get_rng, \&get_saxon, \&get_xsl;
    
    my $out = $file;
    $out =~ s/\.xml$/.html/i;
    
    my $validate = [qw(java -jar), $msvs, "file://localhost/$rng", $file];
    my $compile = [qw(java -jar), $saxon, "-s:$file", "-xsl:$xsl", "-o:$out"];
    
    return ($validate, $compile);
}


main(@ARGV);
