#!/usr/bin/perl

use strict;
use warnings;
use lib 'cgi-bin';
use CGI 'CGI.pm';

my $cgi = CGI->new;
my $upload_dir = $ENV{'UPLOAD_DIR'} || "/home/jules/42/42-webserv/html/static"; 

# Fetching file details from environment variables
my $filename = $ENV{'UPLOAD_FILE_NAME'} || die "Filename is not provided";
my $filetype = $ENV{'UPLOAD_FILE_TYPE'} || die "File type is not provided";
my $filecontent = $ENV{'UPLOAD_FILE_CONTENT'} || die "File content is not provided";

# print "Debug Info:<br>";
# print "UPLOAD_FILE_NAME: $ENV{'UPLOAD_FILE_NAME'}<br>";
# print "UPLOAD_FILE_TYPE: $ENV{'UPLOAD_FILE_TYPE'}<br>";
# print "UPLOAD_FILE_CONTENT: $ENV{'UPLOAD_FILE_CONTENT'}<br>";



print $cgi->header();
print $cgi->start_html('File Upload');

unless (-d $upload_dir) {
    mkdir $upload_dir or die "Unable to create $upload_dir: $!";
}

# Writing file content to a file
open (UPLOADFILE, ">$upload_dir/$filename") or die "Cannot open file for writing: $!";
binmode UPLOADFILE;
print UPLOADFILE $filecontent;
close UPLOADFILE;

print "File \"$filename\" uploaded successfully.";

print $cgi->end_html;

# #!/usr/bin/perl

# use strict;
# use warnings;
# use CGI;

# my $cgi = CGI->new;

# print $cgi->header();
# print $cgi->start_html('Simple Upload Acknowledgement');
# print "Request received.";
# print $cgi->end_html;

