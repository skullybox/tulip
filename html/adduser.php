<?php
   include('session.php');
?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Add Record Form</title>
</head>
<body>
<a href = "welcome.php">Back</a>
<form action="insert.php" method="post">
    <p>
        <label for="name">Name:</label>
        <input type="text" name="name" id="name">
    </p>
    <p>
        <label for="password">Password:</label>
        <input type="password" name="password" id="password">
    </p>
    <p>
        <label for="salt">Salt:</label>
        <input type="text" name="salt" id="salt">
    </p>
    <p>
        <label for="email">Email Address:</label>
        <input type="text" name="email" id="email">
    </p>
    <input type="submit" value="Submit">
</form>
</body>
</html>
