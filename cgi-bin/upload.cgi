#!/usr/bin/perl

use strict;
use warnings;
use lib 'cgi-bin';
use CGI 'CGI.pm';

my $cgi = CGI->new;
my $upload_dir = $ENV{'UPLOAD_DIR'} || "/home/jules/42/42-webserv/html/static"; 

my $filename = $ENV{'UPLOAD_FILE_NAME'} || die "Filename is not provided";
my $filetype = $ENV{'UPLOAD_FILE_TYPE'} || die "File type is not provided";
my $filepath = $ENV{'UPLOAD_FILE_PATH'} || die "File path is not provided";

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
