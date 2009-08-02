#!/usr/bin/perl

# Internal modules:
use Data::Functions;
use Data::Identifier;
use Data::List;
use Data::Set;
use Pearl;


eval {exit main(@ARGV)};
die $EVAL_ERROR;


sub evaluate {
    my ($vars, @tokens) = @ARG;
    die "Missing parentheses on function call.\n" if @tokens == 0;
    
    my %collections = (
        list => {start => '(', end => ')', create => 'Data::List'},
        set => {start => '{', end => '}', create => 'Data::Set'},
    );
    
    if ($tokens[0] =~ m/^\$/) {
        my $variable = shift @tokens;
        
        if (@tokens == 0) {
            my $value = $vars->{$variable};
            defined $value or die "Undefined name: '$variable'.\n";
            return $value;
        }
        elsif ($tokens[0] eq '=') {
            shift @tokens;
            @tokens > 0 or die "Missing assignment value for '$variable'.\n";
            return $vars->{$variable} = evaluate($vars, @tokens);
        }
        
        unshift @tokens, $variable;
    }
    elsif ($tokens[0] =~ m/^&/) {
        my $name = shift @tokens;
        my $function = $FUNCTIONS{$name};
        defined $function or die "Undefined function: '$name'.\n";
        
        my $arguments = evaluate($vars, @tokens);
        my $result = eval {$function->($vars, $arguments->elements())};
        
        die "Function '$name': $EVAL_ERROR" if $EVAL_ERROR;
        return $result;
    }
    elsif ((@tokens == 1) && ($tokens[0] =~ m/^[\p{IsWord}\d]+$/)) {
        return Data::Identifier->new($tokens[0]) if @tokens == 1;
    }
    
    foreach my $name (keys %collections) {
        my $type = $collections{$name};
        
        next unless $tokens[0] eq $type->{start};
        @tokens >= 2 or die "Unterminated $name.\n";
        $tokens[-1] eq $type->{end} or die "\u$name doesn't end properly.\n";
        
        (shift @tokens, pop @tokens);
        my $collection = $type->{create}->new();
        
        while (@tokens > 0) {
            my @removed_tokens;
            
            TOKEN: {
                die $EVAL_ERROR if @tokens == 0;
                push @removed_tokens, shift @tokens;
                eval {$collection->add(evaluate($vars, @removed_tokens))};
                redo TOKEN if $EVAL_ERROR;
            }
        }
        
        return $collection;
    }
    
    die "Invalid syntax: '".join(' ', @tokens)."'\n";
}


sub interpreter {
    my %variables;
    my $prompt = '> ';
    
    print "Type '&help()' for more information:\n$prompt";
    
    for (; my $line = <STDIN>; print $prompt) {
        my @tokens = ($line =~ m/((?:[\$&]?[\p{IsWord}\d]+)|[^\s])/gs);
        next if @tokens == 0;
        
        my @commands;
        my $i = 0;
        
        while (@tokens > 0) {
            my $token = shift @tokens;
            ++$i and next if $token eq ';';
            push @{$commands[$i]}, $token;
        }
        
        foreach my $command (@commands) {
            next unless defined $command;
            my $value = eval {evaluate(\%variables, @$command)};
            $variables{'$'} = $value;
            
            print "$EVAL_ERROR\n" and next if $EVAL_ERROR;
            print $value->to_string(), "\n";
        }
    }
}


sub main {
    interpreter();
    return 0;
}
