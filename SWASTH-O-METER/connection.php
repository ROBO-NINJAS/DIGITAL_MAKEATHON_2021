<?php
     $SERVERNAME = 'localhost';
     $DATABASE_USER = 'root';
     $DATABASE_PASSWORD = '';
     $DATABASE = 'user';

     $con = mysqli_connect($SERVERNAME, $DATABASE_USER, $DATABASE_PASSWORD , $DATABASE);
     if($con){
        echo "Success connection";
    }
    else
    {
        die('Failed to connect to MySQL:'.mysqli_connect_error());
    }

error_reporting(0);
$id = $_POST['ID'];
$N =$_POST['name'];
$E =$_POST['email'];
$PH =$_POST['phone'];
$P =$_POST['password'];
$CP =$_POST['cnfpassword'];

$sqlquery = "INSERT INTO signup VALUES ('$id','$N','$E','$PH','$P','$CP',current_timestamp())";
if($con->query($sqlquery)===TRUE){
    echo "Data inserted in DataBase";
}else{
    echo "Error:".$sqlquery."<br>".$con->error;
}

?>