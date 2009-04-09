package Data::Functions;

use base 'Exporter';
use strict;
use utf8;

use Pearl;


our @EXPORT = qw(%FUNCTIONS);
our $VERSION = 0.1;


our %FUNCTIONS = (
    '&card' => \&card,
    '&cart' => \&cart,
    '&compose' => \&compose,
    '&dif' => \&dif,
    '&dom' => \&dom,
    '&domr' => \&domr,
    '&domar' => \&domar,
    '&equal' => \&equal,
    '&functions' => \&functions,
    '&help' => \&help,
    '&img' => \&img,
    '&in' => \&in,
    '&inv' => \&inv,
    '&inter' => \&inter,
    '&overlap' => \&overlap,
    '&pow' => \&pow,
    '&ran' => \&ran,
    '&ranr' => \&ranr,
    '&ranar' => \&ranar,
    '&subset' => \&subset,
    '&union' => \&union,
    '&vars' => \&vars,
);


sub card {
    my ($vars, $coll) = @ARG;
    my $dom = Data::Set->new();
    
    @ARG == 2
      or die "Expecting 1 argument.\n";
    $coll->isa('Data::Collection')
      or die "First argument must be a collection.\n";
    
    return Data::Identifier->new(scalar $coll->elements());
}


sub cart {
    my ($vars, $set1, $set2) = @ARG;
    my $cart = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $set1->isa('Data::Set') && $set2->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $element1 ($set1->elements()) {
        foreach my $element2 ($set2->elements()) {
            my $tuple = Data::List->new()->add($element1, $element2);
            $cart->add($tuple);
        }
    }
    
    return $cart;
}


sub compose {
    my ($vars, $rel1, $rel2) = @ARG;
    my $compose = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $rel1->isa('Data::Set') && $rel2->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $tuple1 ($rel1->elements()) {
        if (!$tuple1->isa('Data::List') || ($tuple1->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        foreach my $tuple2 ($rel2->elements()) {
            if (!$tuple2->isa('Data::List') || ($tuple2->elements() != 2)) {
                die "A set/relation must contain only 2-tuples.\n";
            }
            
            my ($start1, $end1) = $tuple1->elements();
            my ($start2, $end2) = $tuple2->elements();
            next unless $end1->equals($start2);
            
            my $tuple = Data::List->new()->add($start1, $end2);
            $compose->add($tuple);
        }
    }
    
    return $compose;
}


sub dif {
    my ($vars, $set1, $set2) = @ARG;
    my $dif = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $set1->isa('Data::Set') && $set2->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $element ($set1->elements()) {
        if (!$set2->has($element)) {
            $dif->add($element);
        }
    }
    
    return $dif;
}


sub dom {
    my ($vars, $rel) = @ARG;
    my $dom = Data::Set->new();
    
    @ARG == 2
      or die "Expecting 1 argument.\n";
    $rel->isa('Data::Set')
      or die "First argument must be a set.\n";
    
    foreach my $tuple ($rel->elements()) {
        if (!$tuple->isa('Data::List') || ($tuple->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        my ($element) = $tuple->elements();
        $dom->add($element);
    }
    
    return $dom;
}


sub domr {
    my ($vars, $set, $rel) = @ARG;
    my $domr = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $set->isa('Data::Set') && $rel->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $tuple ($rel->elements()) {
        if (!$tuple->isa('Data::List') || ($tuple->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        my ($element) = $tuple->elements();
        $domr->add($tuple) if $set->has($element);
    }
    
    return $domr;
}


sub domar {
    my ($vars, $set, $rel) = @ARG;
    my $domar = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $set->isa('Data::Set') && $rel->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $tuple ($rel->elements()) {
        if (!$tuple->isa('Data::List') || ($tuple->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        my ($element) = $tuple->elements();
        $domar->add($tuple) unless $set->has($element);
    }
    
    return $domar;
}


sub equal {
    my ($vars, $value1, $value2) = @ARG;
    @ARG == 3 or die "Expecting 2 arguments.\n";
    my $result = $value1->equals($value2) ? $true : $false;
    return Data::Identifier->new($result);
}


sub functions {
    @ARG == 1 or die "Expecting no arguments.\n";
    print "Available functions:\n", join "\n", sort keys %FUNCTIONS;
    return Data::Identifier->new('DONE');
}


sub help {
    @ARG == 1 or die "Expecting no arguments.\n";
    
    print <<'EOT';
This is an interactive interpreter for Set Theory operations (for a list of
available functions type '&functions()' at any time):
- Sets are enclosed in curly brackets, e.g.:
    {a e i o u}
    {1 2 3 2 1}
- Lists are enclosed in parentheses, e.g.:
    (1 3 5 7)
- Variable names start with a '$', e.g.:
    $people = {John Peter}
    &cart($people {Developer Designer})
- The last evaluated value is always available in the special variable '$'.
- Function names start with a '&' and use lists as their arguments, e.g.:
    &functions()
    &union({1 2 3} {3 4 5})
- Use semicolons to separate multiple commands on a single line, e.g.:
    $a = {}; $b = {}
EOT
    
    return Data::Identifier->new('DONE');
}


sub img {
    my ($vars, $rel, $set) = @ARG;
    my $img = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $rel->isa('Data::Set') && $set->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $tuple ($rel->elements()) {
        if (!$tuple->isa('Data::List') || ($tuple->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        my ($start, $end) = $tuple->elements();
        $img->add($end) if $set->has($start);
    }
    
    return $img;
}


sub in {
    my ($vars, $element, $set) = @ARG;
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $set->isa('Data::Set')
      or die "Second argument must be a set.\n";
    
    return Data::Identifier->new($true) if $element->equals(Data::Set->new());
    return Data::Identifier->new($set->has($element));
}


sub inv {
    my ($vars, $rel) = @ARG;
    my $inv = Data::Set->new();
    
    @ARG == 2
      or die "Expecting 1 argument.\n";
    $rel->isa('Data::Set')
      or die "First argument must be a set.\n";
    
    foreach my $tuple ($rel->elements()) {
        if (!$tuple->isa('Data::List') || ($tuple->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        $inv->add(Data::List->new()->add(reverse $tuple->elements()));
    }
    
    return $inv;
}


sub inter {
    my ($vars, $set1, $set2) = @ARG;
    my $inter = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $set1->isa('Data::Set') && $set2->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $element ($set1->elements()) {
        if ($set2->has($element)) {
            $inter->add($element);
        }
    }
    
    return $inter;
}


sub overlap {
    my ($vars, $set1, $set2) = @ARG;
    
    @ARG == 3 or die "Expecting 2 arguments.\n";
    return union($vars,
                 domar($vars,
                       dom($vars, $set2),
                       $set1),
                 $set2);
}


sub pow {
    my ($vars, $set) = @ARG;
    
    @ARG == 2
      or die "Expecting 1 argument.\n";
    $set->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    if ($set->equals(Data::Set->new())) {
        return Data::Set->new()->add(Data::Set->new());
    }
    
    my @elements = $set->elements();
    my $element = pop @elements;
    
    my $pow1 = pow($vars, Data::Set->new()->add(@elements));
    my $pow2 = Data::Set->new();
    
    foreach my $set ($pow1->elements()) {
        $pow2->add(Data::Set->new()->add($set->elements(), $element));
    }
    
    return union($vars, $pow1, $pow2);
}


sub ran {
    my ($vars, $rel) = @ARG;
    my $ran = Data::Set->new();
    
    @ARG == 2
      or die "Expecting 1 argument.\n";
    $rel->isa('Data::Set')
      or die "First argument must be a set.\n";
    
    foreach my $tuple ($rel->elements()) {
        if (!$tuple->isa('Data::List') || ($tuple->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        my (undef, $element) = $tuple->elements();
        $ran->add($element);
    }
    
    return $ran;
}


sub ranr {
    my ($vars, $rel, $set) = @ARG;
    my $ranr = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $rel->isa('Data::Set') && $set->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $tuple ($rel->elements()) {
        if (!$tuple->isa('Data::List') || ($tuple->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        my (undef, $element) = $tuple->elements();
        $ranr->add($tuple) if $set->has($element);
    }
    
    return $ranr;
}


sub ranar {
    my ($vars, $rel, $set) = @ARG;
    my $ranar = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $rel->isa('Data::Set') && $set->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    foreach my $tuple ($rel->elements()) {
        if (!$tuple->isa('Data::List') || ($tuple->elements() != 2)) {
            die "A set/relation must contain only 2-tuples.\n";
        }
        
        my (undef, $element) = $tuple->elements();
        $ranar->add($tuple) unless $set->has($element);
    }
    
    return $ranar;
}


sub subset {
    my ($vars, $set1, $set2) = @ARG;
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $set1->isa('Data::Set') && $set2->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    if (($set1->equals(Data::Set->new())) || ($set1->equals($set2))) {
        return Data::Identifier->new($true);
    }
    
    my $dif = dif($set2, $set1);
    return $false if $dif->equals($set2);
    
    my $result = $dif->equals(Data::Set->new()) ? $false : $true;
    return Data::Identifier->new($result);
}


sub union {
    my ($vars, $set1, $set2) = @ARG;
    my $union = Data::Set->new();
    
    @ARG == 3
      or die "Expecting 2 arguments.\n";
    $set1->isa('Data::Set') && $set2->isa('Data::Set')
      or die "Only sets are accepted.\n";
    
    $union->add($set1->elements());
    $union->add($set2->elements());
    return $union;
}


sub vars {
    my ($vars) = @ARG;
    
    @ARG == 1 or die "Expecting no arguments.\n";
    print "Defined names:\n";
    
    foreach my $var (sort keys %$vars) {
        next unless defined $vars->{$var};
        
        my $value = $vars->{$var}->to_string;
        my $trimmed = substr $value, 0, 50;
        
        print "$var = ", ($value eq $trimmed ? $value : "$trimmed..."), "\n";
    }
    
    return Data::Identifier->new('DONE');
}


1
