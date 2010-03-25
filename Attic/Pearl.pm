package Pearl;

use base qw(Exporter);
use strict;
use threads ();
use utf8;
use warnings;

use Cwd ();
use English qw(-no_match_vars);
use File::Spec ();
use IO::Handle ();


BEGIN {
    if ($OSNAME eq 'MSWin32') {
        # Detect the redirection problem.
        my $in = IO::Handle->new_from_fd(fileno(STDIN), 'r');
        $in or die "Run this script again using the interpreter explicitly.\n";
        $in->close();
    }
}

END {
    $ARG->join() foreach threads->list();
}


our @EXPORT = qw(*STDNULL $false $true abstract async instantiate ls uncapitalize);
our $VERSION = v2009.06.18;


sub abstract {
    my (undef, $filename, $line, $subroutine) = caller(1);
    die "Abstract subroutine &$subroutine called at $filename line $line.\n";
}


sub async(&@) {
    tie my $result, __PACKAGE__.'::Scalar::Lazy', @ARG;
    return \$result;
}


sub import {
    strict->import();
    utf8->import();
    warnings->import();
    
    English->export_to_level(1);
    __PACKAGE__->export_to_level(1);
    
    return 1;
}


sub instantiate {
    my ($invocant, %self) = @ARG;
    my $class = ref($invocant) || $invocant;
    
    return bless \%self, $class;
}


sub ls {
    my ($path) = @ARG;
    $path = Cwd::getcwd() unless defined $path;
    
    opendir my ($directory), $path or die $ERRNO;
    my @files = File::Spec->no_upwards(readdir $directory);
    closedir $directory;
    
    return ((@files == 1) && !wantarray) ? pop @files : @files;
}


sub uncapitalize {
    my ($text) = @ARG;
    
    $text =~ s/(\p{IsWord}+)/length($1) == 1 ? lc($1) : ucfirst(lc($1))/ge;
    return ucfirst $text;
}


open STDNULL, '+<', File::Spec->devnull();

tie our $false, __PACKAGE__.'::Scalar::Constant',
    Pearl::Scalar::Overloaded->new(0, 0, 'false');

tie our $true, __PACKAGE__.'::Scalar::Constant',
    Pearl::Scalar::Overloaded->new(1, 1, 'true');

binmode STDERR;
binmode STDIN;
binmode STDOUT;

autoflush STDERR;
autoflush STDOUT;

$LIST_SEPARATOR = ', ';
$WARNING = $true;


# ------------------------------------------------------------------------------


package Pearl::Scalar::Constant;

use strict;
use warnings;

use Carp ();
use English qw(-no_match_vars);


sub FETCH {
    my ($self) = @ARG;
    return $$self;
}


sub TIESCALAR {
    my ($package) = caller;
    Carp::croak('Internal package') unless $package eq Pearl::;
    
    my ($class, $self) = @ARG;
    return bless \$self, $class;
}


*STORE = *UNTIE = sub {
    Carp::croak('Constant values are read-only');
};


# ------------------------------------------------------------------------------


package Pearl::Scalar::Lazy;

use strict;
use warnings;

use Carp ();
use English qw(-no_match_vars);


sub FETCH {
    my ($self) = @ARG;
    return $self->{value} if exists $self->{value};
    
    $self->{result} = $self->{thread}->join() unless exists $self->{result};
    return $self->{result};
}


sub STORE {
    my ($self, $value) = @ARG;
    $self->{value} = $value;
}


sub TIESCALAR {
    my ($class, $function, @arguments) = @ARG;
    my ($package) = caller;
    
    Carp::croak('Internal package') unless $package eq Pearl::;
    
    my %self = (
        thread => threads->create($function, @arguments),
    );
    
    Carp::croak('Failed to create thread') unless defined $self{thread};
    return bless \%self, $class;
}


sub UNTIE {
    Carp::croak('Lazy scalars must remain tied');
}


# ------------------------------------------------------------------------------


package Pearl::Scalar::Overloaded;

use strict;
use warnings;

use overload
    'fallback' => 1,
    'bool' => \&to_boolean,
    '0+' => \&to_number,
    '""' => \&to_string;

use Carp ();
use English qw(-no_match_vars);


sub new {
    my ($class, $boolean, $number, $string) = @ARG;
    my ($package) = caller;
    
    Carp::croak('Internal package') unless $package eq Pearl::;
    
    my %self = (
        boolean => $boolean,
        number => $number,
        string => $string,
    );
    
    return bless \%self, $class;
}


sub to_boolean {
    my ($self) = @ARG;
    return $self->{boolean};
}


sub to_number {
    my ($self) = @ARG;
    return $self->{number};
}


sub to_string {
    my ($self) = @ARG;
    return $self->{string};
}


1;

__END__

=pod

=head1 SYNOPSIS

    use Pearl;

=head1 DESCRIPTION

Automatically imports commonly used modules (C<English>), turns on essential
pragmas (C<strict>, C<utf8>, C<warnings>), sets some defaults and exports useful
definitions.

=head1 VARIABLES

=over

=item C<STDNULL>

Standard null stream.

=item C<$false>

Contains constant boolean, number and string values for falsehood.

=item C<$true>

Contains constant boolean, number and string values for truth.

=back

=head1 FUNCTIONS

=head2 C<abstract()>

Indicates that a method is abstract and should be implemented.

    sub equals {
        abstract();
    }

=head2 C<async {...} @arguments>

Executes code asynchronously, that is, in a separate thread of execution (if
possible). The resulting value is a reference to a scalar, which points to the
actual result.

    sub greet {
        print "Hello world!\n";
        return 'Bye!';
    }

    my $result = async {greet()};

    print "$result --> $$result\n";
    print "Goodbye!\n";

=head2 C<instantiate($class, %attributes)>

Creates an instance of a class, using the given hash for the initial attributes.

    sub new {
        my ($class, $name, $age) = @ARG;
        return instantiate($class, name => $name, age => $age);
    }

=head2 C<ls($directory)>

Lists all entries in the given directory, or current working directory if not
specified.

    ls("Documents");

=head2 C<uncapitalize($string)>

Removes capitalization of words.

    print uncapitalize("HELLO WORLD!"), "\n";

=head1 AUTHORS

Márcio Faustino
