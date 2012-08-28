#!/usr/bin/perl

use warnings;
use strict;
use Getopt::Long;
use Pod::Usage;
use 5.010;
use Time::localtime;
use Time::HiRes qw(time);

my $HELP = 0;
my $FTYPE = ['reg'];
my $SRCHDEP = -1;
my $ORD = 'pos';
my $SORTBY = 'path';
my $OUTPUT = 'rel';
my $GROUP = 0;
my $DEBUG = 0;

init();

sub init {
    GetOptions(
        'h|help' => \$HELP,
        't|type=s@' => \$FTYPE,
        'd|depth=i' => \$SRCHDEP,
        'o|order=s' => \$ORD,
        's|sort-by=s' => \$SORTBY,
        'p|output=s' => \$OUTPUT,
        'g|group' => \$GROUP,
        'debug=i' => \$DEBUG,
    ) or pod2usage(2);

    debug(2, "\$HELP = $HELP");

    for my $type (@$FTYPE) {
        check_option_value('-t/--type', $type, [qw[reg dir sym]]);
    }
    debug(2, "\$FTYPE = @$FTYPE");

    debug(2, "\$SRCHDEP = $SRCHDEP");

    check_option_value('-o/--order', $ORD, [qw[pos rev nul]]);
    debug(2, "\$ORD = $ORD");

    check_option_value('-s/--sort-by', $SORTBY, [qw[path file size date]]);
    debug(2, "\$SORTBY = $SORTBY");
    debug(2, "\$OUTPUT = $OUTPUT");
    debug(2, "\$GROUP = $GROUP");
    debug(2, "\$DEBUG = $DEBUG");

    pod2usage(1) if $HELP;
}

sub check_option_value {
    my $option = shift;
    my $value = shift;
    my $values = shift;

    unless (grep $value eq $_, @$values) {
        my $optstr = join(', ', @$values);
        print STDERR "Value \"$value\" invalid for option $option (choose from $optstr)\n";
        pod2usage(2);
    }
}

sub debug {
    my $level = shift;
    my $msg = shift;

    if ($level <= $DEBUG) {
        my $msecond = time;
        $msecond =~ s/.*\.(\d{3}).*/$1/;
        my $time_str = sprintf('%02d:%02d:%02d.%03d',
                               localtime->hour, localtime->min, localtime->sec, $msecond);
        say "[DebugMessage $time_str $level] $msg";
    }
}

__END__

=head1 NAME

gfl.pl - Find and print file list by specified path(s).

=head1 SYNOPSIS

gfl.pl [options] [path ...]

  Options:
  -h, --help
  -t {reg,dir,sym}, --type {reg,dir,sym}
  -d DEPTH, --depth DEPTH
  -o {pos,rev,nul}, --order {pos,rev,nul}
  -s {path,file,size,date}, --sort-by {path,file,size,date}
  -p {abs,rel,bsn}, --output {abs,rel,bsn}
  -g, --group
  --debug DEBUG-LEVEL
  -v, --version

=head1 OPTIONS

=over 8

=item B<-h, --help>

Show this help message and exit.

=item B<-t {reg, dir, sym}, --type {reg, dir, sym}>

Specify the file type of the searching.

=over 8

=item {reg}

Regular file.

=item {dir}

Directory.

=item {sym}

Symbolic link.

=back

=item B<-d DEPTH, --depth DEPTH>

Specify the depth of the searching.

=item B<-o {pos,rev,nul}, --order {pos,rev,nul}>

Specify the order of output.

=over 8

=item {pos}

Order to positive .

=item {rev}

Order to reverse.

=item {nul}

Use default order.

=back

=item B<-s {path,file,size,date}, --sort-by {path,file,size,date}>

Specify the key of sort.

=over 8

=item {path}

Sort by path name.

=item {file}

Sort by file name.

=item {size}

Sort by size of file.

=item {date}

Sort by time of last modification.

=back

=item B<-p {abs,rel,bsn}, --optput {abs,rel,bsn}>

Specify the key of sort.

=over 8

=item {abs}

Absolute path.

=item {rel}

Relative path.

=item {bsn}

File name only.

=back

=item B<-g, --group>

Frouping the result.

=item B<--debug DEBUG-LEVEL>

Set the debug level.

=item B<-v, --version>

Display the version of this program and exit.

=back

=cut