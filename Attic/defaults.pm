package defaults;

use autodie;
use base qw(Exporter);
use strict;
use utf8;
use warnings;

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


our @EXPORT = qw(*STDNULL $false $true abstract instantiate);
our $VERSION = v2010.04.15;


sub abstract() {
    my (undef, $file, $line, $subroutine) = caller(1);
    die "Abstract subroutine &$subroutine called at $file line $line.\n";
}


sub import {
    autodie->import();
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


open STDNULL, '+<', File::Spec->devnull();

tie our $false, 'defaults::Scalar::Constant',
    defaults::Scalar::Overloaded->new(0, 0, 'false');

tie our $true, 'defaults::Scalar::Constant',
    defaults::Scalar::Overloaded->new(1, 1, 'true');

binmode STDERR;
binmode STDIN;
binmode STDOUT;

STDERR->autoflush($true);
STDOUT->autoflush($true);

$LIST_SEPARATOR = ', ';
$WARNING = $true;


# ------------------------------------------------------------------------------


package defaults::Scalar::Constant;

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
    Carp::croak('Internal package') unless $package eq defaults::;
    
    my ($class, $self) = @ARG;
    return bless \$self, $class;
}


*STORE = *UNTIE = sub {
    Carp::croak('Constant values are read-only');
};


# ------------------------------------------------------------------------------


package defaults::Scalar::Overloaded;

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
    
    Carp::croak('Internal package') unless $package eq defaults::;
    
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

    use defaults;

=head1 DESCRIPTION

Automatically imports commonly used modules (C<English>), turns on essential
pragmas (C<autodie>, C<strict>, C<utf8>, C<warnings>), sets some defaults and
exports useful definitions.

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

Indicates that a function is abstract and should be implemented.

    sub equals {
        my ($x, $y) = @ARG;
        abstract
    }

=head2 C<instantiate($class, %attributes)>

Creates an instance of a class, using the given hash for the initial attributes.

    sub new {
        my ($class, $name, $age) = @ARG;
        return instantiate($class, name => $name, age => $age);
    }

=head1 AUTHORS

Márcio Moniz Bandim Faustino
