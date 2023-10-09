#!/usr/bin/perl

use strict;
use warnings;
use lib 'cgi-bin';
use CGI 'CGI.pm';

my $cgi = CGI->new;
my $buffer;
my $pair;
my $value;
my %in;

if (length ($ENV{'QUERY_STRING'}) > 0){
    $buffer = $ENV{'QUERY_STRING'};
    my @pairs = split(/&/, $buffer);
    foreach $pair (@pairs){
        my ($name, $value) = split(/=/, $pair);
        $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
        $in{$name} = $value;
    }
}

my $upload_dir = $in{'dir'} || "/home/jules/42/42-webserv/html/static";

my $filename = $in{'name'} || die "Filename is not provided";
my $filepath = $in{'path'} || die "File path is not provided";

print $cgi->start_html('File Upload');

unless (-d $upload_dir) {
    mkdir $upload_dir or die "Unable to create $upload_dir: $!";
}

open (FH, $filepath) or die "Not opening";
binmode FH;
my $filecontent;
while (<FH>)
{
    $filecontent = $filecontent.$_;
}
close FH;

open (UPLOADFILE, ">$upload_dir/$filename") or die "Cannot open file for writing: $!";
binmode UPLOADFILE;
print UPLOADFILE $filecontent;
close UPLOADFILE;

print "File \"$filename\" uploaded successfully.";
print $cgi->end_html;
