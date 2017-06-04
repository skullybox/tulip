<!DOCTYPE html>
<html>
<head>
<style>
label{display:inline-block;width:100px;margin-bottom:10px;}
</style>


<title>Add Employee</title>
</head>
<body>

<?php
$servername = "percona";
$username = "tulip";
$password = "tulip";
$dbname = "tulip";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, firstname, lastname FROM users";
$result = $conn->query($sql);

if ($result->num_rows > 0) {
    // output data of each row
    echo "<DIV>Searching for users..</DIV><BR>";
    while($row = $result->fetch_assoc()) {
        echo "<DIV>Name: " . $row["firstname"]. " " . $row["lastname"]. "</DIV>";
    }
} else {
    echo "0 results";
}
$conn->close();
?>

<HR>
<form method="post" action="insert.php">
<label>First Name</label>
<input type="text" name="first_name" />
<br />
<label>Last Name</label>
<input type="text" name="last_name" />
<br />
<label>Department</label>
<input type="text" name="department" />
<br />
<label>Email</label>
<input type="text" name="email" />

<br />
<input type="submit" value="Add Employee">
</form>



</body>
</html>
