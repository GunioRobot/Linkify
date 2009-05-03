=head1 DESCRIPTION

Automatically imports the English module and defines some defaults.

=head1 SYNOPSIS

  use strict;
  use utf8;
  use Pearl;

=cut
package Pearl;

use base 'Exporter';
use strict;
use threads ();
use utf8;
use warnings;

use Carp;
use English '-no_match_vars';
use File::Spec;
use IO::Handle;


BEGIN {
    # Detect the redirection problem.
    if ($OSNAME eq 'MSWin32') {
        my $io = IO::Handle->new_from_fd(fileno(STDIN), 'r');
        $io or die "Run this script again using the interpreter directly.\n";
        $io->close;
    }
}


our @EXPORT = qw(*STDNULL $false $true async);
our $VERSION = v2009.05.03;


sub import {
    English->export_to_level(1);
    Pearl->export_to_level(1);
    
    return 1;
}


=head1 FUNCTIONS

=over 4

=item async {...};

Executes code asynchronously, that is, in a separate thread of execution (if
possible). The resulting value is a reference to a scalar, which points to the
actual result.

Example:

  sub greet {
      print "Hello world!\n";
      return getlogin;
  }

  my $result = async {greet()};

  print "$result --> $$result\n";
  print "Goodbye!\n";

=back
=cut
sub async(&@) {
    tie my $result, 'Pearl::Lazy::Scalar', @ARG;
    return \$result;
}


=head1 FILEHANDLES

=over 4

=item STDNULL

Standard null stream.

=back
=cut
open STDNULL, '+<', File::Spec->devnull();

=head1 CONSTANTS

=over 4

=item $false

Contains boolean, number and string values for falsehood.
=cut
tie our $false, 'Pearl::Constant::Scalar',
    Pearl::Overloaded::Scalar->new(0, 0, 'false');

=item $true

Contains boolean, number and string values for truth.

=back
=cut
tie our $true, 'Pearl::Constant::Scalar',
    Pearl::Overloaded::Scalar->new(1, 1, 'true');


binmode STDERR, ':utf8';
binmode STDOUT, ':utf8';

autoflush STDERR;
autoflush STDOUT;

$LIST_SEPARATOR = ', ';
$WARNING = $true;


#-------------------------------------------------------------------------------


package Pearl::Constant::Scalar;

use Carp;
use English '-no_match_vars';


sub FETCH {
    my ($self) = @ARG;
    return $$self;
}


sub TIESCALAR {
    my ($package) = caller;
    croak 'Internal package' unless $package eq 'Pearl';
    
    my ($class, $self) = @ARG;
    return bless \$self, $class;
}


*STORE = *UNTIE = sub {
    croak 'Constant values are read-only';
};


#-------------------------------------------------------------------------------


package Pearl::Lazy::Scalar;

use Carp;
use English '-no_match_vars';


sub DESTROY {
    my ($self) = @ARG;
    $self->{thread}->join() unless exists $self->{result};
}


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
    my ($package) = caller;
    croak 'Internal package' unless $package eq 'Pearl';
    
    my ($class, $function, @arguments) = @ARG;
    my $self = {thread => threads->create($function, @arguments)};
    return bless $self, $class;
}


sub UNTIE {
    croak 'Lazy scalars must remain tied';
}


#-------------------------------------------------------------------------------


package Pearl::Overloaded::Scalar;

use Carp;
use English '-no_match_vars';
use overload 'bool' => \&to_boolean, '0+' => \&to_number, '""' => \&to_string;


sub new {
    my ($package) = caller;
    croak 'Internal package' unless $package eq 'Pearl';
    
    my ($class, $boolean, $number, $string) = @ARG;
    my $self = {
        boolean => $boolean,
        number => $number,
        string => $string,
    };
    
    return bless $self, $class;
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


#-------------------------------------------------------------------------------


1
