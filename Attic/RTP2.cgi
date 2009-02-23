#!/usr/bin/perl
# RTP2 TV Guide RSS feed generator.

use strict;
use utf8;
use warnings;

use CGI ':standard';
use Encode;
use File::Basename;
use LWP;
use Storable qw(nstore retrieve);


eval { exit main(@ARGV) };
print "Content-type: text/plain; charset=UTF-8\n\n", "Error: $@\n";
exit 1;


sub date {
    my ($seconds, $minutes, $hours, $day, $month, $year) = gmtime;
    
    $year += 1900;
    ++$month;
    
    if (wantarray) {
        return ($year, $month, $day);
    }
    else {
        return sprintf '%d-%02d-%02d', $year, $month, $day;
    }
}


sub download_guide {
    my ($date) = @_;
    my $browser = LWP::UserAgent->new;
    my $response = $browser->get(url_for($date), 'User-Agent' => 'Mozilla');
    
    $response->is_success
      or die 'URL error: '.$response->status_line;
    $response->content_type eq 'text/html'
      or die 'Invalid content type: '.$response->content_type;
    
    return Encode::decode 'ISO-8859-1', $response->content;
}


sub generate_rss {
    my ($guide) = @_;
    
    print <<'EOT';
Content-type: text/xml; charset=UTF-8

<?xml version="1.0"?>
<rss version="2.0" xmlns:dc="http://purl.org/dc/elements/1.1/"
                   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
  <channel>
    <title>RTP2 Guia TV</title>
    <link>http://rtp2.rtp.pt/guia.php</link>
EOT
    
    foreach my $date (sort keys %{$guide->{content}}) {
        print <<"EOT";
    
    <item>
      <title>Programação para $date</title>
      <guid>$date</guid>
      <description>
      <![CDATA[
EOT
        
        foreach my $program (@{$guide->{content}{$date}}) {
            print <<"EOT";
        <b>$program->{hour}</b> - $program->{title}<br/>
EOT
        }
        
        print <<'EOT';
      ]]>
      </description>
    </item>
EOT
    }
    
    print <<'EOT';
  </channel>
</rss>
EOT
}


sub load_guide {
    my ($file) = @_;
    my $content = -e $file ? retrieve($file) : {};
    my $today = date();
    
    unless (exists $content->{$today}) {
        $content->{$today} = parse_html(download_guide($today));
    }
    
    return {file => $file, content => $content};
}


sub main {
    binmode STDOUT, ':utf8';
    
    if ((url_param('view') || '') eq 'source') {
        open my $self, '<:utf8', $0 or die $!;
        print "Content-type: text/plain; charset=UTF-8\n\n";
        print while <$self>;
        close $self or die $!;
    }
    else {
        my ($name) = fileparse($0, '.cgi');
        my $guide = load_guide("$name.rss");
        
        generate_rss($guide);
        save_guide($guide);
    }
    
    return 0;
}


sub parse_html {
    my ($content) = @_;
    
    $content =~ m/<!-- INICIO GRELHA -->(.+)<!-- FIM GRELHA -->/s
      or die 'Could not find TV guide';
    
    my @lines = ($1 =~ m#<span[^>]+"guia(\w+?)"[^>]*>\s*(.+?)\s*</span>#gs);
    my @guide;
    
    while (@lines > 0) {
        shift @lines, my $hour = shift @lines;
        shift @lines, my $title = shift @lines;
        
        while ((@lines > 0) && ($lines[0] eq 'Programas')) {
            shift @lines, shift @lines;
        }
        
        $title =~ s#.*?<a[^>]*>\s*(.+?)\s*</a>.*#$1#s;    # Extract text.
        $title =~ s/(\p{IsWord}+)/\u\L$1/g;               # Uncapitalize.
        
        push @guide, {hour => $hour, title => $title};
    }
    
    return \@guide;
}


sub save_guide {
    my ($guide) = @_;
    my $content = $guide->{content};
    my $guides = 0;
    
    foreach my $date (reverse sort keys %$content) {
        delete $content->{$date} if ++$guides >= 7;
    }
    
    nstore($content, $guide->{file});
}


sub url_for {
    my ($date) = @_;
    $date = join '-', reverse split m/-/, $date;
    
    #return "http://rtp2.rtp.pt/epg_output.php?dia=$date";
    return "http://www.rtp.pt/tv/rtp2/includes/epg_output.php?dia=$date";
}
