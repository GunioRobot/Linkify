#!/usr/bin/perl

# Creates a wrapper to dynamically load/unload a library.

# External modules:
use Class::Struct ();
use Env::Path ();
use File::Basename ();
use File::Slurp ();
use File::Spec ();
use Regexp::Common qw(balanced comment);

# Internal modules:
use strict;
use utf8;
use warnings;


my ($directory, $include) = @ARGV;
my ($include_dir) = grep {-e File::Spec->catfile($_, $include)} include_dirs();
my $include_file = File::Spec->catfile($include_dir, $include);

my $name = 'wrapper';
my @namespace = File::Spec->splitdir(File::Spec->canonpath($directory));
my ($library) = File::Basename::fileparse($include_file, '.h');

Class::Struct::struct Wrapper => {
    header => '$',
    include => '$',
    include_file => '$',
    library => '$',
    name => '$',
    namespace => '@',
};

my $wrapper = Wrapper->new(
    header => sprintf('__%s__', uc join '__', @namespace, $name),
    include => $include,
    include_file => $include_file,
    library => $library,
    name => $name,
    namespace => \@namespace,
);

implementation($wrapper);
interface($wrapper);


sub functions {
    my ($file) = @_;
    my $text = File::Slurp::read_file($file);
    my $comments = $Regexp::Common::RE{comment}{'C++'};
    my $parenthesis = $Regexp::Common::RE{balanced}{-parens => '()'};
    my @functions;
    
    $text =~ s/$comments//g;
    
    while ($text =~ m/(\w+)\s+(\w+)\s*($parenthesis)\s*;/cg) {
        my ($return, $name, $args) = ($1, $2, $3);
        
        $args =~ s/\s{2,}/ /g;
        $args =~ s/\s*([()])\s*/$1/g;
        
        push @functions, {return => $return, name => $name, args => $args};
    }
    
    return @functions;
}


sub implementation {
    my ($wrapper) = @_;
    my @namespace = @{$wrapper->namespace()};
    my ($header, $name) = ($wrapper->header(), $wrapper->name());
    my $library = $wrapper->library();
    
    open my $output, '>', File::Spec->catfile(@namespace, "$name.cpp") or die $!;
    print $output (<< "EOT");
#include "$name.h"


#undef $header
#define API_FUNCTION_DEFINITION
#include "$name.h"
#undef API_FUNCTION_DEFINITION

#undef $header
#define API_FUNCTION_LOADER
#include "$name.h"
#undef API_FUNCTION_LOADER

#undef $header
#define API_FUNCTION_UNLOADER
#include "$name.h"
#undef API_FUNCTION_UNLOADER


static HINSTANCE _library = NULL;


EOT
    
    foreach my $part (@namespace) {
        print $output "namespace $part {\n";
    }
    
    print $output (<< "EOT");
    HINSTANCE Load() {
        if (_library == NULL) {
            _library = LoadLibrary(TEXT("$library"));
            
            if (_library != NULL) {
                LoadFunctions(_library);
            }
        }
        
        return _library;
    }
    
    
    bool Unload() {
        if (_library != NULL) {
            if (FreeLibrary(_library) == 0) {
                return false;
            }
            
            _library = NULL;
            UnloadFunctions();
        }
        
        return true;
    }
EOT
    
    print $output '}' x @namespace, "\n";
    close $output;
}


sub include_dirs {
    my %path_lists;
    my %paths;
    
    foreach my $vcproj (glob '*.vcproj') {
        my $text = File::Slurp::read_file($vcproj);
        @path_lists{$text =~ m/AdditionalIncludeDirectories\s*=\s*"([^"]*)"/g} = 1;
    }
    
    foreach my $path_list (keys %path_lists) {
        $path_list =~ s/&quot;//g;
        
        foreach my $path (split Env::Path::PathSeparator(), $path_list) {
            eval {
                $path =~ s/\$\(([^)]+)\)/$ENV{$1} || die/eg;
                $paths{$path} = 1;
            };
        }
    }
    
    return keys %paths;
}


sub interface {
    my ($wrapper) = @_;
    my @namespace = @{$wrapper->namespace()};
    my ($header, $name) = ($wrapper->header(), $wrapper->name());
    my $include = $wrapper->include();
    
    open my $output, '>', File::Spec->catfile(@namespace, "$name.h") or die $!;
    print $output (<< "EOT");
#ifndef $header
#define $header


#include "../../Core.h"
#include <$include>


#if defined(API_FUNCTION_DEFINITION)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \\
        name##Function name = NULL
#elif defined(API_FUNCTION_LOADER)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \\
        (name = (name##Function) GetProcAddress(library, TEXT(#name)))
#elif defined(API_FUNCTION_UNLOADER)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \\
        (name = NULL)
#else
#   define API_FUNCTION(return ,name, args) \\
        typedef return (*name##Function) args; \\
        extern name##Function name
#endif


EOT
    
    foreach my $part (@namespace) {
        print $output "namespace $part {\n";
    }
    
    print $output (<< 'EOT');
    HINSTANCE Load();
    bool Unload();
    
#if defined(API_FUNCTION_LOADER)
static void LoadFunctions(HINSTANCE library) {
#elif defined(API_FUNCTION_UNLOADER)
static void UnloadFunctions() {
#endif
    
EOT
    
    foreach my $function (functions($wrapper->include_file())) {
        my ($return, $name, $args) = @$function{qw(return name args)};
        print $output (<< "EOT");
    API_FUNCTION($return, $name, $args);
EOT
    }
    
    print $output (<< 'EOT');
    
#if defined(API_FUNCTION_LOADER) || defined(API_FUNCTION_UNLOADER)
}
#endif
EOT
    
    print $output '}' x @namespace, "\n";
    print $output (<< 'EOT');


#undef API_FUNCTION


#endif
EOT
    close $output;
}
