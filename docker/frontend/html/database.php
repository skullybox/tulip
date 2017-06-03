<?php
$connect=mysqli_connect('localhost','tulip','tulip123','tulip');

if(mysqli_connect_errno($connect))
{
        echo 'Failed to connect';
}

?>
