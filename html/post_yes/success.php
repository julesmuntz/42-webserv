<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        body {
            background-image: url("static/stage.png");
        }
    </style>
</head>
<body>
    
</body>
</html>

<?php 

    echo "<p>Votre username est : ".$_POST['username']." </p>" ;
    echo "<p>Votre password est : ".$_POST['password']." </p>" ;
    echo "<p>Votre email est : ".$_POST['email']." </p>" ;
    echo "<p>Votre gender est : ".$_POST['gender']." </p>" ;
    echo "<p>Votre interests est : ".$_POST['interests']." </p>" ;
    echo "<p>Votre comments est : ".$_POST['comments']." </p>" ;
    echo $_POST['fileUpload'];
?>
