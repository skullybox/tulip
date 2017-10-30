<?php
   include('session.php');
?>
<html>
<style>
table, th, td {
    border: 1px solid black;
    border-collapse: collapse;
}
th, td {
    padding: 15px;
}
</style>


<?php
$servername = "localhost";
$username = "tulip";
$password = "TuliPP455#";
$dbname = "tulip";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "select name,email from user";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // output data of each row
    echo "<table><tr><th>Name</th><th>email</th></tr>";
    while($row = $result->fetch_assoc()) {
        $name = $row["name"];
        $bgcolor = gray;
        if ($name == terry) { $bgcolor = lightgreen; }
        if ($name == irfan) { $bgcolor = red; }
        
        echo "<tr><td bgcolor=$bgcolor>" . $name. "</td>";
        echo "<td>" . $row["email"]. "</td></tr>";
    }
} else {
    echo "0 results";
}
$conn->close();
?>
<a href = "welcome.php">Back</a>
</html>

