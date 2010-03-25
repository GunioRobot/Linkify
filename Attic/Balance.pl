#!/usr/bin/perl

# External modules:
use autodie;
use Class::Struct ();
use Digest ();
use Encode ();
use File::Basename ();
use File::Copy ();
use File::Spec ();
use File::Temp ();
use Getopt::Long ();
use IO::Compress::Bzip2 ();
use IO::Uncompress::Bunzip2 ();
use Statistics::Descriptive ();
use Text::CSV::Slurp ();

# Internal modules:
use Pearl;


Class::Struct::struct Database => {
    path => '$',
    fields => '@',
    records => '%',
};


sub import_cgd_csv {
    my ($database, $file) = @ARG;
    
    foreach my $record (parse_cgd_csv($file)) {
        my $date = $record->{'Data Valor'};
        my $name = $record->{Descrição};
        my ($debit, $credit) = @{$record}{qw(Débito Crédito)};
        my $amount = $debit || $credit;
        my $id = Digest->new('SHA-256');
        
        $date =~ s/^(\d\d)-(\d\d)-(\d{4})$/$3-$2-$1/;
        $amount =~ s/\.//g;
        $amount =~ s/,/./g;
        $amount = sprintf "%s€", ($debit eq '') ? (0 + $amount) : (0 - $amount);
        
        $id->add("$date\0$amount\0$name\0");
        $id->add("$record->{'Data movimentos'}\0");
        $id->add("$record->{'Saldo contabilístico'}\0");
        $id = $id->b64digest();
        
        unless (exists $database->records()->{$id}) {
            $database->records()->{$id} = {
                Date => $date,
                Amount => $amount,
                Name => $name,
                Description => '',
                Owner => '',
                BIC => 'CGDIPTPL',
                ID => $id,
                Extra => '',
            }
        }
    }
}


sub main {
    my ($help, $pause, $import, $path) = ($false, $false);
    
    return unless Getopt::Long::GetOptions(
        'help' => \$help,
        'pause' => \$pause,
        'import' => \$import,
        'database=s' => \$path);
    
    if ($help) {
        print <<'USAGE' and return;
Options:
  --help
  --pause
  --import CSV
  --database path
USAGE
    }
    
    unless (defined $path) {
        my ($name, $dir) = File::Basename::fileparse($PROGRAM_NAME, '.pl');
        $path = File::Spec->catpath('', $dir, "$name.csv.bz2");
    }
    
    my $database = open_database($path);
    
    if ($import) {
        import_cgd_csv($database, $ARG) foreach @ARGV;
        save_database($database);
    }
    else {
        statistics($database, \*STDOUT);
    }
    
    if ($pause) {
        <STDIN>;
    }
}


sub open_database {
    my ($path) = @ARG;
    my @fields = qw(Date Amount Name Description Owner BIC ID Extra);
    my $database = Database->new(path => $path, fields => \@fields);
    my $file;
    
    eval {open $file, '<', $path};
    
    if ($EVAL_ERROR) {
        $database->records({});
    }
    else {
        IO::Uncompress::Bunzip2::bunzip2($file, \my $data);
        close $file;
        
        my $records = Text::CSV::Slurp->load(string => Encode::decode_utf8($data));
        $database->records({map {($ARG->{ID} => $ARG)} @$records});
    }
    
    return $database;
}


sub parse_cgd_csv {
    my ($file_name) = @ARG;
    open my $file, '<:encoding(iso-8859-1)', $file_name;
    
    <$file> =~ m/^"Data";"(\d\d-){2}\d{4}"$/ or die 'Not a CGD style CSV.';
    my $contents = join '', grep !m/^\s+$/, <$file>;
    close $file;
    
    return @{Text::CSV::Slurp->load(string => $contents, sep_char => ';')};
}


sub save_database {
    my ($database) = @ARG;
    my @records = sorted_records($database);
    
    if (@records > 0) {
        my ($file, $path) = File::Temp::tempfile(SUFFIX => '.csv.bz2');
        my $records = Encode::encode_utf8(
            Text::CSV::Slurp->create(
                input => \@records,
                field_order => $database->fields()));
        
        binmode $file;
        IO::Compress::Bzip2::bzip2(\$records, $file);
        close $file;
        
        File::Copy::move($path, $database->path());
    }
}


sub sorted_records {
    my ($database) = @ARG;
    return sort {$a->{Date} cmp $b->{Date}} values %{$database->records()};
}


sub statistics {
    my ($database, $output) = @ARG;
    my @records = grep {$ARG->{Amount} =~ m/^-/} sorted_records($database);
    
    if (@records == 0) {
        print "No data.\n";
        return;
    }
    
    my $spendings = Statistics::Descriptive::Sparse->new();
    my $daily_spendings = Statistics::Descriptive::Sparse->new();
    my $monthly_spendings = Statistics::Descriptive::Sparse->new();
    my $yearly_spendings = Statistics::Descriptive::Sparse->new();
    
    my %daily_spendings;
    my %monthly_spendings;
    my %yearly_spendings;
    
    foreach my $record (@records) {
        my ($amount) = ($record->{Amount} =~ m/(.+)€$/);
        my $date = $record->{Date};
        my ($year, $month) = ($date =~ m/^(\d{4})-(\d\d)/);
        
        $spendings->add_data($amount);
        $daily_spendings{$date} += $amount;
        $monthly_spendings{$month} += $amount;
        $yearly_spendings{$year} += $amount;
    }
    
    my ($last_year, $last_month, $last_day) = split '-', $records[-1]->{Date};
    my @last_year = grep {$ARG->{Date} =~ m/^$last_year/} @records;
    my @last_month = grep {$ARG->{Date} =~ m/-$last_month-/} @last_year;
    my @last_day = grep {$ARG->{Date} =~ m/$last_day$/} @last_month;
    
    $last_year = Statistics::Descriptive::Sparse->new();
    $last_month = Statistics::Descriptive::Sparse->new();
    $last_day = Statistics::Descriptive::Sparse->new();
    
    $last_year->add_data(map {$ARG->{Amount} =~ m/(.+)€$/} @last_year);
    $last_month->add_data(map {$ARG->{Amount} =~ m/(.+)€$/} @last_month);
    $last_day->add_data(map {$ARG->{Amount} =~ m/(.+)€$/} @last_day);
    
    $daily_spendings->add_data(values %daily_spendings);
    $monthly_spendings->add_data(values %monthly_spendings);
    $yearly_spendings->add_data(values %yearly_spendings);
    
    my $statistics = <<'EOT';
Spendings:
  Average:
    Mean: %.2f
    Daily: %.2f
    Monthly: %.2f
    Yearly: %.2f
  Milestone:
    Day: %.2f
    Month: %.2f
    Year: %.2f
    Lowest: %.2f
    Highest: %.2f
EOT
    
    printf $output $statistics,
        $spendings->mean(),
        $daily_spendings->mean(),
        $monthly_spendings->mean(),
        $yearly_spendings->mean(),
        $last_day->sum(),
        $last_month->sum(),
        $last_year->sum(),
        $spendings->max(),
        $spendings->min();
}


main();
