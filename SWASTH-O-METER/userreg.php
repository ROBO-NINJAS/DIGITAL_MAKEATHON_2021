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
    $N = $_POST['name'];
    $E = $_POST['email'];
    $G = $_POST['rdbgender'];
    $PH = $_POST['phone'];
    $ADD = $_POST['address'];
    $PW = $_POST['password'];
    $CNFPW = $_POST['cnfpassword'];

    $sqlquery = "INSERT INTO userreg values ('$id','$N','$E','$G','$PH','$ADD','$PW','$CNFPW',current_timestamp())";
    if($con->query($sqlquery)===TRUE){
        echo 'DATA INSERTED';
    }else{
        echo "ERROR".$sqlquery."<br>".$con->error;
    }

?>