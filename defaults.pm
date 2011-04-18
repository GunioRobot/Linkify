package defaults;

use base qw(Exporter autodie);
use feature qw(:5.10);
use strict;
use true;
use utf8;
use warnings;

use Carp ();
use English qw(-no_match_vars);
use File::Spec ();
use IO::Handle ();
use Try::Tiny ();


our @EXPORT = qw(*STDNULL $false $true const);
our $VERSION = v2011.04.17;


# TODO: Use first one available of Const::Fast, Data::Lock, Readonly::XS?
sub const :lvalue {
    Carp::croak('Not a scalar variable') if @ARG != 1;
    Internals::SvREADONLY($ARG[0], 1);
    $ARG[0];
}


sub import {
    feature->import(qw(:5.10));
    strict->import();
    true->import();
    utf8->import();
    warnings->import();
    
    __PACKAGE__->export_to_level(1);
    English->export_to_level(1);
    Try::Tiny->export_to_level(1);
    
    # No export available.
    goto &autodie::import;
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


=pod

=encoding UTF-8

=head1 SYNOPSIS

    use defaults;

=head1 DESCRIPTION

Automatically imports commonly used modules, turns on essential pragmas, sets
some defaults and exports useful definitions.

=head1 IMPORTS

=over

=item C<autodie>

=item C<feature qw(:5.10)>

=item C<strict>

=item C<true>

=item C<utf8>

=item C<warnings>

=item C<English qw(-no_match_vars)>

=item C<Try::Tiny>

=back

=head1 VARIABLES

=head2 C<STDNULL>

Standard null stream.

=head2 C<$false>

Constant for falsehood.

=head2 C<$true>

Constant for truth.

=head1 FUNCTIONS

=head2 C<const SCALAR>

Indicates that a scalar variable is read-only.

    const my $BUFFER_SIZE = 64;

=head1 AUTHORS

Márcio Faustino
