#!/usr/bin/perl

# External programs:
# - Java: <http://java.sun.com/javase/downloads/>
# - xmllint: <http://xmlsoft.org/xmldtd.html#validate1>
# - xmlto: <http://cyberelk.net/tim/software/xmlto/>

use defaults;
use Archive::Extract ();
use Crypt::SSLeay ();
use Cwd ();
use File::Path ();
use File::Slurp ();
use File::Spec ();
use LWP::UserAgent ();
use XML::DOM ();


my $NAMESPACE = 'http://docbook.org/ns/docbook';
my $PUBLIC_ID = qr{-//OASIS//DTD\s+DocBook\s+XML\s+V([\d.]+)//EN};


sub detect_version {
    my ($file) = @ARG;
    my $doc = eval {XML::DOM::Parser->new()->parsefile($file)};
    
    if ($EVAL_ERROR) {
        my $xml = File::Slurp::read_file($file, scalar_ref => $true);
        
        if ($$xml =~ m/\bxmlns\s*=\s*"\Q$NAMESPACE\E"/) {
            my @versions = ($$xml =~ m/<[^<>?]*\bversion\s*=\s*"([^"]+)"/g);
            return pop @versions if @versions == 1;
        }
        elsif ($$xml =~ m/"$PUBLIC_ID"/) {
            return $1;
        }
    }
    else {
        if ($doc->getDocumentElement()->getAttribute('xmlns') eq $NAMESPACE) {
            return $doc->getDocumentElement()->getAttribute('version');
        }
        elsif (my $doctype = $doc->getDoctype()) {
            return $1 if $doctype->getPubId() =~ m/^$PUBLIC_ID$/;
        }
    }
    
    return;
}


sub download {
    my ($link, $output) = @ARG;
    my $browser = LWP::UserAgent->new();
    my %options = ('User-Agent' => 'Mozilla');
    
    if (defined $output) {
        $options{':content_file'} = $output;
    }
    
    my $response = $browser->get($link, %options);
    die $response->status_line() . "\n" unless $response->is_success();
    
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
        my $url = $base_url . $leaf_url;
        my $file = File::Basename::fileparse($url);
        
        File::Path::mkpath($cache);
        
        my $path = download($url, File::Spec->catfile($cache, $file));
        my $archive = Archive::Extract->new(archive => $path);
        
        $archive->extract(to => $cache) or die $archive->error() . "\n";
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
        
        $archive->extract(to => $cache) or die $archive->error() . "\n";
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
        
        $archive->extract(to => $cache) or die $archive->error() . "\n";
        unlink $path;
    }
    
    my $root = File::Spec->catdir($cache, ls($cache));
    my $xhtml = File::Spec->catdir($root, 'xhtml');
    
    return File::Spec->catfile($xhtml, 'docbook.xsl');
}


sub ls {
    my ($path) = @ARG;
    $path = Cwd::getcwd() unless defined $path;
    
    opendir my ($directory), $path;
    my @files = File::Spec->no_upwards(readdir $directory);
    closedir $directory;
    
    return ((@files == 1) && !wantarray) ? pop @files : @files;
}


sub main {
    my ($file) = @ARG;
    my $version;
    
    if (defined $file) {
        $version = detect_version($file);
    }
    else {
        foreach my $xml_file (grep m/\.xml$/i, ls()) {
            $version = detect_version($xml_file);
            
            if (defined $version) {
                $file = $xml_file;
                print "Automatic detection: DocBook v$version: $file\n";
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
Usage: [file]
USAGE
    }
    
    return;
}


sub publish {
    my ($file, $version) = @ARG;
    my $publish = sprintf 'publish_v%u', $version;
    my ($validate, $compile) = __PACKAGE__->can($publish)->($file);
    
    print "\n";
    system @$validate;
    print "\n";
    system @$compile;
    
    return;
}


sub publish_v4 {
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
    $out =~ s/\.xml$/.html/i;
    
    my ($msvs, $rng, $saxon, $xsl) = @data{qw(msvs rng saxon xsl)};
    my $validate = ['java', '-jar', $msvs, "file://localhost/$rng", $file];
    my $compile = ['java', '-jar', $saxon, "-s:$file", "-xsl:$xsl", "-o:$out"];
    
    return ($validate, $compile);
}


main(@ARGV);
