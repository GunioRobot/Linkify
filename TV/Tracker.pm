package TV::Tracker;

use defaults;
use File::Spec ();
use LWP::UserAgent ();
use Module::Load ();


sub _download {
    my ($self, $url, $cookies) = @ARG;
    my $agent = LWP::UserAgent->new();
    
    $agent->cookie_jar($cookies) if defined $cookies;
    return $agent->get($url, 'User-Agent' => 'Mozilla')->decoded_content();
}


sub get_status {
    my ($self, $show, $season, $episode) = @ARG;
    abstract
}


sub list_episodes {
    my ($self, $show, $season) = @ARG;
    abstract
}


sub list_seasons {
    my ($self, $show) = @ARG;
    abstract
}


sub list_shows {
    my ($self) = @ARG;
    abstract
}


sub list_trackers {
    my ($class) = @ARG;
    my $dir = do {$ARG = __FILE__, s/\.pm//, $ARG};
    
    opendir my ($trackers), $dir;
    my @trackers = grep {-f File::Spec->catfile($dir, $ARG)} readdir $trackers;
    closedir $trackers;
    
    return sort map {s/\.pm//; $ARG} @trackers;
}


sub load_tracker {
    my ($class, $name, @args) = @ARG;
    my $module = sprintf '%s::%s', __PACKAGE__, $name;
    
    Module::Load::load($module);
    return $module->new(@args);
}


sub name {
    my ($self) = @ARG;
    abstract
}


sub new {
    my ($class, @args) = @ARG;
    abstract
}


1;
