Bonjour, <?php echo $_POST['username']; ?>.
Ton email est <?php echo $_POST['email']; ?>.

<?php 
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'];
    $email = $_POST['email'];
    echo "eeee $username";
    // Utilisez $username et $email comme vous le souhaitez
}

?>