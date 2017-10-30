<?php
   define('DB_SERVER', 'localhost');
   define('DB_USERNAME', 'tulip');
   define('DB_PASSWORD', 'TuliPP455#');
   define('DB_DATABASE', 'tulip');
   $db = mysqli_connect(DB_SERVER,DB_USERNAME,DB_PASSWORD,DB_DATABASE);

// Check connection
if ($db->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

?>
