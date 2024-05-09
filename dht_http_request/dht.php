<?php
include_once("dbconnect.php");
$temp = $_GET['temp'];
$hum = $_GET['hum'];

$sqlinsertdht = "INSERT INTO `tbl_dht`(`temperature`, `humidity`) VALUES ('$temp','$hum ')";
if ($conn->query($sqlinsertdht) === TRUE){
    echo "success";
}else {
    echo "failed";
}

?>
