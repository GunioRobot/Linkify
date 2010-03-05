#!/usr/bin/perl

# Creates a wrapper to dynamically load/unload a library.

# External modules:
use File::Basename;
use File::Slurp;
use File::Spec;
use Regexp::Common;

# Internal modules:
use strict;
use warnings;


my $wrapper = 'wrapper';
my $main = shift @ARGV;

my ($library, $path, $suffix) = fileparse($main, '.h');
my @namespace = File::Spec->splitdir(File::Spec->canonpath($path));
my $header = sprintf '__%s__', uc join '__', @namespace, $wrapper;

open my $impl, '>', File::Spec->catfile(@namespace, "$wrapper.cpp") or die $!;
implementation($impl);
close $impl;

open my $interface, '>', File::Spec->catfile(@namespace, "$wrapper.h") or die $!;
interface($interface);
close $interface;


sub functions {
    my $text = read_file($main);
    my @functions;
    
    $text =~ s/$RE{comment}{'C++'}//g;
    
    while ($text =~ /(\w+)\s+(\w+)\s*($RE{balanced}{-parens => '()'})\s*;/cg) {
        my ($return, $name, $args) = ($1, $2, $3);
        
        $args =~ s/\s{2,}/ /g;
        $args =~ s/\s*([()])\s*/$1/g;
        
        push @functions, {return => $return, name => $name, args => $args};
    }
    
    return @functions;
}


sub implementation {
    my ($output) = @_;
    
    print $output (<< "EOT");
#include "$wrapper.h"


#undef $header
#define API_FUNCTION_DEFINITION
#include "wrapper.h"
#undef API_FUNCTION_DEFINITION

#undef $header
#define API_FUNCTION_LOADER
#include "wrapper.h"
#undef API_FUNCTION_LOADER

#undef $header
#define API_FUNCTION_UNLOADER
#include "wrapper.h"
#undef API_FUNCTION_UNLOADER


static HINSTANCE _library = NULL;


EOT
    
    foreach my $name (@namespace) {
        print $output "namespace $name {\n";
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
}


sub interface {
    my ($output) = @_;
    
    print $output (<< "EOT");
#ifndef $header
#define $header


#include "../../Core.h"
#include "$library.h"


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
    
    foreach my $name (@namespace) {
        print $output "namespace $name {\n";
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
    
    foreach my $function (functions()) {
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
}
