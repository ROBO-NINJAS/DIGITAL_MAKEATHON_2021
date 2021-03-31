<?php

    $SERVERNAME = "localhost";
    $USERNAME = "root";
    $PASSWORD = "";
    $DBNAME = "user";

    $con = mysqli_connect($SERVERNAME, $USERNAME, $PASSWORD, $DBNAME);
    if($con){
        echo 'success connection';
    }else{
        die('failed to connect'.mysqli_connect_error());
    }
    error_reporting(0);
    $id = $_POST['ID'];
    $N = $_POST['username'];
    $PW = $_POST['password'];

    $sqlquery = "INSERT INTO userlogin values ('$id','$N','$PW',current_timestamp())";
    if($con->query($sqlquery)===TRUE){
        echo 'DATA INSERTED';
    }else{
        echo "ERROR".$sqlquery."<br>".$con->error;
    }

?>