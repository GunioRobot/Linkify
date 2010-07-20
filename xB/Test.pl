#!/usr/bin/perl

use defaults;
use File::Basename ();
use File::Slurp ();
use File::Spec ();
use HTTP::Daemon ();
use HTTP::Response ();
use HTTP::Status ();


sub main {
    my $running = $true;
    my $server;
    
    foreach my $port (80, 8008, 8080, 8090) {
        $server = HTTP::Daemon->new(LocalPort => $port, Timeout => 1) and last;
    }
    
    defined $server or die "Busy.\n";
    printf "[%s] On-line: %s\n", now(), $server->url();
    
    $SIG{INT} = sub {
        printf "[%s] Stopping...\n", now();
        $running = $false;
    };
    
    while ($running) {
        if (my $client = $server->accept()) {
            reply($client, $ARG) while $ARG = $client->get_request();
            close $client;
        }
    }
    
    close $server if defined $server;
    printf "[%s] Off-line.\n", now();
}


sub module {
    my ($path) = @ARG;
    my $response = HTTP::Response->new(HTTP::Status::HTTP_OK);
    my $contents = File::Slurp::read_file($path,
        binmode => ':raw', scalar_ref => $true);
    
    $response->header('Content-Type' => 'text/javascript; charset=UTF-8');
    $response->content_ref($contents);
    
    return $response;
}


sub modules {
    my ($module_suffix, $test_suffix) = @ARG;
    my %depends;
    
    foreach my $path (glob '*' . $module_suffix) {
        my $contents = File::Slurp::read_file($path,
            binmode => ':raw', scalar_ref => $true);
        
        my @requires = ($contents =~ m'@requires\s+([^\s]+)'g);
        my $name = File::Basename::basename($path, $test_suffix, $module_suffix);
        
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
        ($year + 1900), ($month + 1), $day, $hours, $minutes, $seconds;
}


sub reply {
    my ($client, $request) = @ARG;
    my $module_suffix = '.js';
    my $test_suffix = '.test' . $module_suffix;
    
    printf "[%s] Request: %s\n", now(), $request->uri();
    
    unless ($request->method() eq 'GET') {
        $client->send_error(HTTP::Status::HTTP_FORBIDDEN);
        return;
    }
    
    my ($volume, $directory, $file) = File::Spec->splitpath($request->uri());
    
    if (($volume eq '') && ($directory eq '/')) {
        if ($file eq '') {
            $client->send_response(test($module_suffix, $test_suffix));
            return;
        }
        elsif (($file =~ m/\Q$module_suffix\E$/) && stat($file)) {
            $client->send_response(module($file));
            return;
        }
    }
    
    $client->send_error(HTTP::Status::HTTP_NOT_FOUND);
    return;
}


sub test {
    my ($module_suffix, $test_suffix) = @ARG;
    my $response = HTTP::Response->new(HTTP::Status::HTTP_OK);
    
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
    
    foreach my $name (modules($module_suffix, $test_suffix)) {
        $response->add_content_utf8(<<"HTML");
    
    <h2>$name</h2>
    <script src="$name$module_suffix" type="text/javascript"></script>
    <script src="$name$test_suffix" type="text/javascript"></script>
HTML
    }
    
    $response->add_content_utf8(<<'HTML');
  </body>
</html>
HTML
    
    return $response;
}


main(@ARGV);
