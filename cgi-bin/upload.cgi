#!/usr/bin/perl

use strict;
use warnings;
use CGI;

my $cgi = CGI->new;
my $upload_dir = "/home/jules/42/42-webserv/html/static";

print $cgi->header();
print $cgi->start_html('File Upload');

if ($cgi->param('fileUpload')) {
    my $filename = $cgi->param('fileUpload');
    $filename =~ s/.*[\/\\](.*)/$1/;
    my $upload_filehandle = $cgi->upload('fileUpload');

    unless (-d $upload_dir) {
        mkdir $upload_dir or die "Unable to create $upload_dir: $!";
    }

    open (UPLOADFILE, ">$upload_dir/$filename") or die "$!";
    binmode UPLOADFILE;
    while (<$upload_filehandle>) {
        print UPLOADFILE;
    }
    close UPLOADFILE;
    
    print "File \"$filename\" uploaded successfully.";
} else {
    print "No file uploaded.";
}

print $cgi->end_html;
