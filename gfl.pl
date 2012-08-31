#!/usr/bin/perl

use warnings;
use strict;
use Getopt::Long;
use Pod::Usage;
use 5.010;
use Cwd;
use File::Spec;
use Time::localtime;
use Time::HiRes qw(time);

my $HELP = 0;
my $FTYPE = ['reg'];
my $SRCHDEP = -1;
my $REV = 0;
my $SORTBY = 'path';
my $OUTPUT = 'rel';
my $GROUP = 0;
my $DEBUG = 0;

my @paths = init();

my %file_list;
for my $path (@paths) {
    chomp $path;
    push @{$file_list{$path}}, list_dir($path, $SRCHDEP, @$FTYPE);
}

my @sorted_list;
if ($SORTBY ne 'null') {
    if ($SORTBY eq 'path') {
        push @sorted_list, 
             [$_, [sort {
                       File::Spec->catfile($a->{$SORTBY}, $a->{file})
                       cmp
                       File::Spec->catfile($b->{$SORTBY}, $b->{file})
                   } @{$file_list{$_}}]]
            for (@paths);
    }
    elsif ($SORTBY eq 'file') {
        push @sorted_list, [$_, [sort {$a->{file} cmp $b->{file}} @{$file_list{$_}}]] for (@paths);
    }
    elsif ($SORTBY eq 'size' || $SORTBY eq 'date') {
        push @sorted_list, [$_, [sort {$a->{$SORTBY} <=> $b->{$SORTBY}} @{$file_list{$_}}]] for (@paths);
    }
}
else {
    push @sorted_list, [$_, $file_list{$_}] for (@paths);
}

my $frist_g = 1;
for my $path (@sorted_list) {
    if ($GROUP) {
        print "\n" unless $frist_g;
        print @$path[0]."\n";
        $frist_g = 0;
    }

    my $fmt_path = File::Spec->catdir(File::Spec->splitdir(@$path[0]));
    debug(2, "\$fmt_path = $fmt_path");

    @{@$path[1]} = reverse @{@$path[1]} if ($REV);

    for my $file (@{@$path[1]}) {
        if ($OUTPUT eq 'abs') {
            print File::Spec->catfile($file->{path}, $file->{file})."\n";
        }
        elsif ($OUTPUT eq 'rel') {
            print substr(
                      File::Spec->catfile($file->{path}, $file->{file}),
                      length($fmt_path) + 1,
                  )."\n";
        }
        elsif ($OUTPUT eq 'bsn') {
            print $file->{file}."\n";
        }
    }
}

sub init {
    GetOptions(
        'h|help' => \$HELP,
        't|type=s@' => \$FTYPE,
        'd|depth=i' => \$SRCHDEP,
        'r|reverse' => \$REV,
        's|sort-by=s' => \$SORTBY,
        'o|output=s' => \$OUTPUT,
        'g|group' => \$GROUP,
        'debug=i' => \$DEBUG,
    ) or pod2usage(2);

    my @paths = @ARGV ? @ARGV : (cwd());

    debug(2, "\$HELP = $HELP");

    for my $type (@$FTYPE) {
        check_option_value('-t/--type', $type, qw[reg dir sym]);
    }
    debug(2, "\$FTYPE = @$FTYPE");

    debug(2, "\$SRCHDEP = $SRCHDEP");

    debug(2, "\$REV = $REV");

    check_option_value('-s/--sort-by', $SORTBY, qw[path file size date null]);
    debug(2, "\$SORTBY = $SORTBY");

    check_option_value('-o/--output', $OUTPUT, qw[abs rel bsn]);
    debug(2, "\$OUTPUT = $OUTPUT");

    debug(2, "\$GROUP = $GROUP");
    debug(2, "\$DEBUG = $DEBUG");
    debug(2, "\＠paths = @paths");

    pod2usage(1) if $HELP;

    return @paths;
}

sub check_option_value {
    my $option = shift;
    my $value = shift;
    my @values = @_;

    unless (grep $value eq $_, @values) {
        my $optstr = join(', ', map "'$_'", @values);
        print STDERR "Value \"$value\" invalid for option $option (choose $optstr)\n";
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
        say STDERR "[DebugMessage $time_str $level] $msg";
    }
}

sub list_dir {
    my $path = shift;
    my $depth = shift;
    my @ftype = @_;

    my @ret_list;

    if ($depth) {
        if (-d $path) {
            debug(1, "\$path = $path");
            local *DH;
            if (!opendir(DH, $path)) {
                print STDERR "Cannot opendir $path: $!\n";
                return;
            }

            for (readdir(DH)) {
                next if ($_ eq '.' || $_ eq '..');

                my $file = File::Spec->catfile($path, $_);

                if (-d $file) {
                    debug(4, 'Enter path: '.$file);
                    push @ret_list, list_dir($file, $depth - 1, @ftype);
                    next unless (grep $_ eq 'dir', @ftype);
                }
                else {
                    debug(4, "Found file: $file");

                    next unless (grep($_ eq 'reg', @ftype) && -f $file && !-l $file
                        or grep($_ eq 'sym', @ftype) && -l $file);
                }

                debug(1, "$file");
                my ($mode, $size, $mtime) = (lstat($file))[2, 7, 9];
                debug(2, "Size: $size");
                debug(2, "Mtime: $mtime");
                push @ret_list, {
                    path => $path,
                    file => $_,
                    size => $size,
                    date => $mtime,
                };
            }

            close(DH);
        }
        else {
            print STDERR "[ERROR] Can not found path: $path\n"
        }
    }
    else {
        debug(1, "Out of depth on path: $path");
    }

    return @ret_list;
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
  -r, --reverse
  -s {path,file,size,date}, --sort-by {path,file,size,date,null}
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

=item B<-r, --reverse>

Reverse the result.

=item B<-s {path,file,size,date}, --sort-by {path,file,size,date,null}>

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

=item {null}

Use default order.

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
