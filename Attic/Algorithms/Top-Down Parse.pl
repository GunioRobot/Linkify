#!/usr/bin/perl

use Pearl;
use Class::Struct;


struct State => {
    syms => '@',
    pos => '$',
};

sub is_terminal {
    my ($symbol) = @ARG;
    return $symbol ne uc $symbol;
}

sub state_of {
    my ($state) = @ARG;
    my ($symbols, $position) = ($state->syms, $state->pos + 1);
    
    return "((@$symbols) $position)";
}

print 'Input: ';
chomp(my $input = <STDIN>);

print "\nGrammar:\n";
my (@targets, @unknown_targets);
my %grammar;

READ_RULES: {
    print '  ';
    chomp(my $rule = <STDIN>);
    
    if ($rule) {
        my ($target, $symbols) = ($rule =~ m/^(\w+)\s*->([\s\w]+)$/);
        my @symbols = ($symbols =~ /(\w+)/g);
        
        push @targets, $target unless exists $grammar{$target};
        push @{$grammar{$target}}, \@symbols;
        
        @unknown_targets = grep {!$grammar{$ARG}} @unknown_targets;
        push @unknown_targets, grep {!is_terminal($ARG) && !$grammar{$ARG}} @symbols;
        redo READ_RULES;
    }
    elsif (@unknown_targets > 0) {
        warn "Missing symbols: @unknown_targets\n";
        redo READ_RULES;
    }
    
    redo READ_RULES if @targets == 0;
};

print "Lexicon:\n";
my @words = ($input =~ m/(\w+)/g);
my %lexicon;

foreach my $word (@words) {
    next if exists $lexicon{$word};
    print "  \"$word\": ";
    $lexicon{$word} = [<STDIN> =~ m/(\w+)/g];
}

my $current = State->new();
my $use_dfs = $true;
my @alternatives;

$current->syms([$targets[0]]);
$current->pos(0);

for (my $i = 1; (@{$current->syms} > 0) || ($current->pos != @words); ++$i) {
    my $symbols = $current->syms;
    my $position = $current->pos;
    
    print "\n# Current ($i): ", state_of($current), "\n";
    print '# Alternatives: (', join(' ', map {state_of($ARG)} @alternatives), ")\n";
    
    my $target = shift @$symbols;
    
    if (defined($target)) {
        if (is_terminal($target)) {
            my $word = $words[$position];
            my @categories = grep {$ARG eq $target} @{$lexicon{$word}};
            
            if ((@categories == 1) && ($categories[0] eq $target)) {
                $current->pos($position + 1);
                next;
            }
        }
        else {
            my ($rule, @rest) = @{$grammar{$target}};
            my @states;
            
            foreach my $rule (@rest) {
                my $state = State->new();
                $state->syms([@$rule, @$symbols]);
                $state->pos($position);
                
                push @states, $state;
            }
            
            if ($use_dfs) {
                unshift @alternatives, reverse @states;
            }
            else { # BFS:
                push @alternatives, reverse @states;
            }
            
            unshift @$symbols, @$rule;
            next;
        }
    }
    
    print "\n-- FAIL! --\n";
    die "No more alternatives.\n" if @alternatives == 0;
    $current = shift @alternatives;
}

print "\nFinal: ", state_of($current), "\n";
