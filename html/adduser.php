<?php
   include('session.php');
?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<base target="body">
<title>Add Record Form</title>
</head>
<body>
<form action="insert.php" method="post">
    <table><tr><td><label for="name">Name:</label></td/><td><input type="text" name="name" id="name"></td></tr>
           <tr><td><label for="password">Password:</label></td><td><input type="password" name="password" id="password"></td></tr>
           <tr><td><label for="salt">Salt:</label></td><td><input type="text" name="salt" id="salt"></td></tr>
           <tr><td><label for="email">Email:</label></td><td><input type="text" name="email" id="email"></td></tr></table>
    <input type="submit" value="Submit">
</form>
</body>
</html>
