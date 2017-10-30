<?php
   include('session.php');
?>
<html">
   
   <head>
      <title>Welcome </title>
   </head>
   
   <body>
      <table><tr><td><img src="logo.jpeg" alt="Smiley face" height="168" width="340"></td>
      <td style="font-size: 25px;">Welcome, <font color=lightblue>(<?php echo $login_session; ?>)</font><br><br><br>
      <a href = "adduser.php">Add a User</a>  
      <a href = "showusers.php">List Users</a>
      <a href = "logout.php">Sign Out</a></td></table>
      
   </body>
   
</html>
