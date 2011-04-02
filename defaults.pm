package defaults;

use base qw(Exporter autodie);
use strict;
use utf8;
use warnings;

use English qw(-no_match_vars);
use File::Spec ();
use IO::Handle ();


our @EXPORT = qw(*STDNULL $false $true abstract const instantiate);
our $VERSION = v2011.04.02;


sub abstract() {
    my (undef, $file, $line, $subroutine) = caller(1);
    die "Abstract subroutine &$subroutine called at $file line $line.\n";
}


sub const :lvalue {
    Internals::SvREADONLY($_[0], 1);
    $_[0];
}


sub import {
    strict->import();
    utf8->import();
    warnings->import();
    
    English->export_to_level(1);
    __PACKAGE__->export_to_level(1);
    
    # No export available.
    goto &autodie::import;
}


sub instantiate {
    my ($invocant, %self) = @ARG;
    my $class = ref($invocant) || $invocant;
    
    return bless \%self, $class;
}


const our $false = 0;
const our $true = 1;

open STDNULL, '+<', File::Spec->devnull();

binmode STDERR;
binmode STDIN;
binmode STDOUT;

STDERR->autoflush($true);
STDOUT->autoflush($true);

$LIST_SEPARATOR = ', ';
$WARNING = $true;


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

Constant for falsehood.

=item C<$true>

Constant for truth.

=back

=head1 FUNCTIONS

=head2 C<abstract>

Indicates that a function is abstract and should be implemented.

    sub equals {
        my ($x, $y) = @ARG;
        abstract
    }

=head2 C<const SCALAR>

Indicates that a scalar variable is read-only.

    const my $BUFFER_SIZE = 64;

=head2 C<instantiate($class, %attributes)>

Creates an instance of a class, using the given hash for the initial attributes.

    sub new {
        my ($class, $name, $age) = @ARG;
        
        return instantiate($class,
            name => $name,
            age => $age);
    }

=head1 AUTHORS

Márcio Faustino
