<html><body><base target="body"></body></html>
<?php
include('session.php');
include("config.php");
 
// Escape user inputs for security
$name = mysqli_real_escape_string($db, $_REQUEST['name']);
$password = mysqli_real_escape_string($db, $_REQUEST['password']);
$salt = mysqli_real_escape_string($db, $_REQUEST['salt']);
$email = mysqli_real_escape_string($db, $_REQUEST['email']);

if (empty($name)) {
    echo '$name is either 0, empty, or not set at all';
    header("location: adduser.php");
    exit;
}
if (empty($password)) {
    echo '$password is either 0, empty, or not set at all';
    header("location: adduser.php");
    exit;
}
if (empty($salt)) {
    echo '$salt is either 0, empty, or not set at all';
    header("location: adduser.php");
    exit;
}
if (empty($email)) {
    echo '$email is either 0, empty, or not set at all';
    header("location: adduser.php");
    exit;
}

 
// attempt insert query execution
$sql = "INSERT INTO user (name, password, salt, email) VALUES ('$name', '$password', '$salt', '$email')";
if(mysqli_query($db, $sql)){
    echo "Records added successfully.<br>";
    header("location: body.php");
    
} else{
    echo "ERROR: Could not able to execute $sql. " . mysqli_error($link);
}
 
// close connection
mysqli_close($link);
?>
