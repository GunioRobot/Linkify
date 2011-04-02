package TV::Tracker::MyEpisodes;
use base qw(TV::Tracker);

use defaults;
use HTTP::Cookies ();
use HTTP::Request::Common ();
use List::MoreUtils ();
use LWP::UserAgent ();
use Regexp::Common qw(whitespace);


my $base_url = 'http://www.myepisodes.com/';


sub __cache_episodes {
    my ($self, $show) = @ARG;
    
    return if exists $self->{cache}{$show};
    my $list = $self->_download("views.php?type=epsbyshow&showid=$show");
    
    my @season_names = ($list =~ m/class="season">([^<]+)/gi);
    my @season_ids = ($list =~ m/id="epscnt([^"]+)/gi);
    my %seasons = List::MoreUtils::mesh(@season_ids, @season_names);
    
    my @episode_names = ($list =~ m|target="_blank">([^<]+)</a></td>|gi);
    my @episode_ids = ($list =~ m/id="A([^"]+)/gi);
    my %episodes = List::MoreUtils::mesh(@episode_ids, @episode_names);
    
    my %acquired = ($list =~ m|name="A([^"]+)"((?:\s+checked)?)></td>|gi);
    my %viewed = ($list =~ m|name="V([^"]+)"((?:\s+checked)?)></td>|gi);
    my %season_episodes;
    my %status;
    
    foreach my $season (@season_ids) {
        $season_episodes{$season} = [grep m/^\Q$season\E/, @episode_ids];
    }
    
    foreach my $episode (@episode_ids) {
        my $status = 'unseen';
        
        if ($viewed{$episode} =~ m/checked/i) {
            $status = 'seen';
        }
        elsif ($acquired{$episode} =~ m/checked/i) {
            $status = 'acquired';
        }
        
        $status{$episode} = $status;
    }
    
    $self->{cache}{$show}{seasons} = \%seasons;
    $self->{cache}{$show}{episodes} = \%episodes;
    $self->{cache}{$show}{season_episodes} = \%season_episodes;
    $self->{cache}{$show}{status} = \%status;
}


sub __log_in {
    my ($self, $name, $password) = @ARG;
    my $agent = LWP::UserAgent->new();
    my $cookies = HTTP::Cookies->new();
    
    my $log_in = HTTP::Request::Common::POST($base_url . 'login.php', [
        username => $name,
        password => $password,
        action => 'Login',
        u => '',
    ]);
    
    $agent->cookie_jar($cookies);
    
    my $content = $agent->request($log_in)->decoded_content();
    my ($warning) = ($content =~ m|<div\s+class="warning">(.+?)</div>|si);
    
    $warning ||= '';
    $warning =~ s/<[^>]+>//g;
    $warning =~ s/$Regexp::Common::RE{ws}{crop}//g;
    
    die "$warning.\n" unless $warning eq '';
    $self->{cookies} = $cookies;
    
    return $self;
}


sub _download {
    my ($self, $url) = @ARG;
    return $self->SUPER::_download($base_url . $url, $self->{cookies});
}


sub get_status {
    my ($self, $show, $season, $episode) = @ARG;
    
    $self->__cache_episodes($show);
    return $self->{cache}{$show}{status}{$episode};
}


sub list_episodes {
    my ($self, $show, $season) = @ARG;
    $self->__cache_episodes($show);
    
    my @ids = @{$self->{cache}{$show}{season_episodes}{$season}};
    my @names = @{$self->{cache}{$show}{episodes}}{@ids};
    
    return List::MoreUtils::mesh(@ids, @names);
}


sub list_seasons {
    my ($self, $show) = @ARG;
    
    $self->__cache_episodes($show);
    return %{$self->{cache}{$show}{seasons}};
}


sub list_shows {
    my ($self) = @ARG;
    my $shows = $self->_download('shows.php?type=manage');
    
    return $shows =~ m/<option\s+value="([^"]+)">([^<]+)/gi;
}


sub name {
    my ($self) = @ARG;
    return 'MyEpisodes';
}


sub new {
    my $class = shift @ARG;
    die "Arguments: <user name> <user password>\n" if @ARG != 2;
    
    return bless({}, $class)->__log_in(@ARG);
}


1;
