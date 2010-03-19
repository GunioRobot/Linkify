#!/usr/bin/perl

# External programs:
# - Java: <http://java.sun.com/javase/downloads/>
# - xmllint: <http://xmlsoft.org/xmldtd.html#validate1>
# - xmlto: <http://cyberelk.net/tim/software/xmlto/>

# External modules:
use Archive::Extract ();
use Crypt::SSLeay ();
use File::Path ();
use File::Spec ();
use LWP::UserAgent ();
use XML::DOM ();

# Internal modules:
use Pearl;


eval {exit main(@ARGV)};
print "Error: $EVAL_ERROR\n";
exit 1;


sub download {
    my ($link, $output) = @ARG;
    my $browser = LWP::UserAgent->new();
    my %options = ('User-Agent' => 'Mozilla');
    
    if (defined $output) {
        $options{':content_file'} = $output;
    }
    
    my $response = $browser->get($link, %options);
    die $response->status_line() unless $response->is_success();
    
    return defined $output ? $output : $response->decoded_content();
}


sub get_msvs {
    my ($cache_root) = @ARG;
    my $cache = File::Spec->catdir($cache_root, 'MSVS');
    
    unless (-e $cache) {
        print "Downloading Multi-Schema Validator Schematron add-on...\n";
        
        my $base_url = 'https://msv.dev.java.net';
        my $list = "$base_url/servlets/ProjectDocumentList?folderID=101";
        my ($leaf_url) = (download($list) =~ m/"([^"]+relames[^"]+\.zip)"/);
        my $url = $base_url.$leaf_url;
        my $file = File::Basename::fileparse($url);
        
        File::Path::mkpath($cache);
        
        my $path = download($url, File::Spec->catfile($cache, $file));
        my $archive = Archive::Extract->new(archive => $path);
        
        $archive->extract(to => $cache) or die $archive->error();
        unlink $path;
    }
    
    my $root = File::Spec->catdir($cache, ls($cache));
    return File::Spec->catfile($root, 'relames.jar');
}


sub get_rng {
    my ($cache_root) = @ARG;
    my $cache = File::Spec->catdir($cache_root, 'RNG');
    
    unless (-e $cache) {
        print "Downloading DocBook RELAX NG schema...\n";
        
        my $url = 'http://www.docbook.org/xml/5.0/rng/docbook.rng';
        my $file = File::Basename::fileparse($url);
        
        File::Path::mkpath($cache);
        download($url, File::Spec->catfile($cache, $file));
    }
    
    return File::Spec->catfile($cache, ls($cache));
}


sub get_saxon {
    my ($cache_root) = @ARG;
    my $cache = File::Spec->catdir($cache_root, 'Saxon');
    
    unless (-e $cache) {
        print "Downloading Saxon XSLT processor...\n";
        
        my $list = 'http://saxon.sourceforge.net/';
        my ($file) = (download($list) =~ m/(saxonhe[^j"]+j\.zip)/);
        my $url = "http://prdownloads.sourceforge.net/saxon/$file?download";
        
        File::Path::mkpath($cache);
        
        my $path = download($url, File::Spec->catfile($cache, $file));
        my $archive = Archive::Extract->new(archive => $path);
        
        $archive->extract(to => $cache) or die $archive->error();
        unlink $path;
    }
    
    return File::Spec->catfile($cache, 'saxon9he.jar');
}


sub get_xsl {
    my ($cache_root) = @ARG;
    my $cache = File::Spec->catdir($cache_root, 'XSL');
    
    unless (-e $cache) {
        print "Downloading DocBook XSL-NS style sheets...\n";
        
        my $base = 'http://docbook.sourceforge.net/release/xsl-ns';
        my $info = "$base/current/VERSION";
        my ($version) = (download("$base/current/VERSION") =~ m/<fm:Version>([^<]+)/);
        my $file = "docbook-xsl-ns-$version.tar.bz2";
        my $url = "http://prdownloads.sourceforge.net/docbook/$file?download";
        
        File::Path::mkpath($cache);
        
        my $path = download($url, File::Spec->catfile($cache, $file));
        my $archive = Archive::Extract->new(archive => $path);
        
        $archive->extract(to => $cache) or die $archive->error();
        unlink $path;
    }
    
    my $root = File::Spec->catdir($cache, ls($cache));
    my $xhtml = File::Spec->catdir($root, 'xhtml');
    
    return File::Spec->catfile($xhtml, 'docbook.xsl');
}


sub is_docbook {
    my ($file) = @ARG;
    my $document = XML::DOM::Parser->new()->parsefile($file);
    my $namespace = $document->getDocumentElement()->getAttribute('xmlns');
    
    if ($namespace eq 'http://docbook.org/ns/docbook') {
        return $true;
    }
    if (my $doctype = $document->getDoctype()) {
        return $doctype->getPubId() =~ m|^-//OASIS//DTD DocBook XML|;
    }
    
    return $false;
}


sub main {
    if (@ARG == 0) {
        my @docbook_files = grep {m/\.xml$/ && is_docbook($ARG)} ls();
        
        if (@docbook_files == 1) {
            my ($file) = @docbook_files;
            print "Auto-detected file: $file\n";
            return main($file);
        }
    }
    
    if (@ARG != 1) {
        print "Compiles documents in DocBook format to HTML.\n";
        print "Usage: [document file]\n";
        return 1;
    }
    
    my ($file) = @ARG;
    my $document = XML::DOM::Parser->new()->parsefile($file);
    my $version = $document->getDocumentElement()->getAttribute('version');
    my $publish = ($version =~ m/^5/) ? \&publish_v5 : \&publish;
    
    $document->dispose();
    my ($validate, $compile) = $publish->($file);
    
    print "Validating...\n";
    system @$validate;
    
    print "Compiling...\n";
    system @$compile;
    
    return 0;
}


sub publish {
    my ($file) = @ARG;
    my $validate = ['xmllint', '--noout', '--valid', $file];
    my $compile = ['xmlto', 'html-nochunks', $file];
    
    return ($validate, $compile);
}


sub publish_v5 {
    my ($file) = @ARG;
    my $cache = File::Spec->catdir($ENV{USERPROFILE} || $ENV{HOME}, '.DocBook~');
    my $out = $file;
    
    my %data = (
        msvs => \&get_msvs,
        rng => \&get_rng,
        saxon => \&get_saxon,
        xsl => \&get_xsl,
    );
    
    $data{$ARG} = $data{$ARG}->($cache) for sort keys %data;
    $out =~ s/\.xml$/.html/;
    
    my ($msvs, $rng, $saxon, $xsl) = @data{qw(msvs rng saxon xsl)};
    my $validate = ['java', '-jar', $msvs, "file://localhost/$rng", $file];
    my $compile = ['java', '-jar', $saxon, "-s:$file", "-xsl:$xsl", "-o:$out"];
    
    return ($validate, $compile);
}
