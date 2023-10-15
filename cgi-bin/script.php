#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");

$buffer = getenv('QUERY_STRING');
$in = [];

if (strlen($buffer) > 0) {
    $pairs = explode("&", $buffer);
    foreach ($pairs as $pair) {
        list($name, $value) = explode("=", $pair);
        $value = urldecode($value);
        $in[$name] = $value;
    }
}

$upload_dir = isset($in['dir']) ? $in['dir'] : '/home/jules/42/42-webserv/html/static';
$filename = isset($in['name']) ? $in['name'] : die("Filename is not provided");
$filepath = isset($in['path']) ? $in['path'] : die("File path is not provided");

echo "<html><head><title>File Upload</title></head><body>";

if (!file_exists($upload_dir)) {
    mkdir($upload_dir) or die("Unable to create $upload_dir");
}

$filecontent = file_get_contents($filepath) or die("Unable to read the file");
file_put_contents("$upload_dir/$filename", $filecontent) or die("Cannot write to the destination");

echo "File \"$filename\" uploaded successfully.";
echo "</body></html>";
?>

<!-- #!/usr/bin/php

<?php

$buffer = $_SERVER['QUERY_STRING'];
$in = [];

if (strlen($buffer) > 0) {
    $pairs = explode("&", $buffer);
    foreach ($pairs as $pair) {
        list($name, $value) = explode("=", $pair);
        $value = urldecode($value);
        $in[$name] = $value;
    }
}

$upload_dir = isset($in['dir']) ? $in['dir'] : '/home/jules/42/42-webserv/html/static';
$filename = isset($in['name']) ? $in['name'] : die("Filename is not provided");
$filepath = isset($in['path']) ? $in['path'] : die("File path is not provided");

echo "<html><head><title>File Upload</title></head><body>";

if (!file_exists($upload_dir)) {
    mkdir($upload_dir) or die("Unable to create $upload_dir");
}

$filecontent = file_get_contents($filepath) or die("Not opening");
file_put_contents("$upload_dir/$filename", $filecontent) or die("Cannot open file for writing");

echo "File \"$filename\" uploaded successfully.";
echo "</body></html>";
?> -->
