#!/usr/bin/perl

use File::Basename qw(basename);
use File::Spec::Functions qw(catfile splitpath);
use File::stat;
use FindBin;
use HTTP::Daemon;
use HTTP::Response;
use HTTP::Status qw(HTTP_NOT_FOUND RC_FORBIDDEN RC_OK);
use Pearl;


my $server = HTTP::Daemon->new(LocalPort => 80, Timeout => 1) or die "Busy.\n";
my $client;

my $mtime = stat($PROGRAM_NAME)->mtime();
my $running = $true;

my $module_suffix = '.js';
my $test_suffix = '.test'.$module_suffix;

printf "[%s] On-line: %s\n", now(), $server->url();

$SIG{INT} = sub {
    printf "[%s] Stopping...\n", now();
    $running = $false;
};

while ($running) {
    $client = $server->accept() or next;
    
    while (my $request = $client->get_request()) {
        printf "[%s] Request: %s\n", now(), $request->uri();
        
        if ($request->method() ne 'GET') {
            $client->send_error(RC_FORBIDDEN);
            next;
        }
        
        my ($volume, $directory, $file) = splitpath($request->uri());
        
        if (($volume eq '') && ($directory eq '/')) {
            if ($file eq '') {
                $client->send_response(test());
                next;
            }
            elsif (($file =~ m/\Q$module_suffix\E$/) && stat($file)) {
                $client->send_response(script($file));
                next;
            }
        }
        
        $client->send_error(HTTP_NOT_FOUND);
    }
}
continue {
    close $client if defined $client;
    
    if ($mtime != stat($PROGRAM_NAME)->mtime()) {
        printf "[%s] Reloading server...\n", now();
        undef $mtime;
        last;
    }
}

close $server if defined $server;
printf "[%s] Off-line.\n", now();
exec catfile($FindBin::Bin, basename($PROGRAM_NAME)), @ARGV unless $mtime;


sub modules {
    my %depends;
    
    foreach my $path (glob '*'.$module_suffix) {
        open my $file, '<:encoding(UTF-16)', $path;
        my @requires = join('', <$file>) =~ m/\@requires\s+([^\s]+)/g;
        close $file;
        
        my $name = basename($path, $test_suffix, $module_suffix);
        push @{$depends{$name}}, @requires;
    }
    
    my @unordered = keys %depends;
    my @ordered;
    
    while (@unordered > 0) {
        my $name = shift @unordered;
        next unless exists $depends{$name};
        
        if (@{$depends{$name}} == 0) {
            delete $depends{$name};
            push @ordered, $name;
        }
        else {
            push @unordered, $name;
            unshift @unordered, shift @{$depends{$name}};
        }
    }
    
    return @ordered;
}


sub now {
    my ($seconds, $minutes, $hours, $day, $month, $year) = localtime;
    
    return sprintf '%d-%02d-%02d %02d:%02d:%02d',
        ($year + 1900), ($month + 1), $day,
        $hours, $minutes, $seconds;
}


sub script {
    my ($path) = @ARG;
    my $response = HTTP::Response->new(RC_OK);
    
    open my $file, '<:raw', $path;
    my $contents = join '', <$file>;
    close $file;
    
    $response->header('Content-Type' => 'text/javascript; charset=UTF-16BE');
    $response->content_ref(\$contents);
    return $response;
}


sub test {
    my $response = HTTP::Response->new(RC_OK);
    
    $response->header('Content-Type' => 'text/html; charset=UTF-8');
    $response->add_content_utf8(<<'HTML');
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" lang="en">
  <head>
    <title>Test</title>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
    <meta name="Author" content="Márcio Faustino"/>
  </head>
  
  <body>
    <script type="text/javascript">
// <![CDATA[
function test(label, tests) {
    var numberTests = 0;
    var failures = [];
    
    if (arguments.length == 1) {
        tests = label;
        label = undefined;
    }
    
    for (var name in tests) {
        try {
            ++numberTests;
            tests[name]();
        }
        catch (exception) {
            failures[failures.length] = {exception: exception, name: name};
        }
    }
    
    var successes = numberTests - failures.length;
    
    if (label) {
        document.write('<h4>' + label + '</h4>');
    }
    
    if (successes > 0) {
        document.write('<p>Successes: ' + successes + '</p>');
    }
    
    if (failures.length > 0) {
        document.write('<p>Failures: ' + failures.length + '</p>');
        document.write('<ul>');
        
        for (var i = 0; i < failures.length; ++i) {
            var what = failures[i].name;
            var why = failures[i].exception.message;
            
            document.write('<li><em>' + what + '</em>: <q>' + why + '</q></li>');
        }
        
        document.write('</ul>');
    }
}
// ]]>
    </script>
HTML
    
    foreach my $name (modules()) {
        $response->add_content_utf8(<<"HTML");
    
    <h2>$name</h2>
    <script src="$name$module_suffix" type="text/javascript"></script>
    <script src="$name$test_suffix" type="text/javascript"></script>
HTML
    }
    
    $response->add_content_utf8(<<"HTML");
  </body>
</html>
HTML
    
    return $response;
}
