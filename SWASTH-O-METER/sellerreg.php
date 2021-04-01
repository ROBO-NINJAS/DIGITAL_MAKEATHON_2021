<?php
    $SERVERNAME = "localhost";
    $USERNAME = "root";
    $PASSWORD = "";
    $DBNAME = "seller";

    $con = mysqli_connect($SERVERNAME, $USERNAME, $PASSWORD, $DBNAME);
    if($con){
        echo 'success connection';
    }else{
        die('failed to connect'.mysqli_connect_error());
    }

    if(isset($_POST['submit'])){
        error_reporting(0);

        $id = mysqli_real_escape_string($con,$_POST['ID']);
        $SHPN = mysqli_real_escape_string($con,$_POST['shopname']);
        $gst = mysqli_real_escape_string($con,$_POST['gstin']);
        $SHPADD = mysqli_real_escape_string($con,$_POST['shopaddress']);
        $N = mysqli_real_escape_string($con,$_POST['name']);
        $E = mysqli_real_escape_string($con,$_POST['email']);
        $G = mysqli_real_escape_string($con,$_POST['rdbgender']);
        $PH = mysqli_real_escape_string($con,$_POST['phone']);
        $ADD = mysqli_real_escape_string($con,$_POST['address']);
        $PW = mysqli_real_escape_string($con,$_POST['password']);
        $CNFPW = mysqli_real_escape_string($con,$_POST['cnfpassword']);

        $pass = password_hash($PW,PASSWORD_BCRYPT);
        $cpass = password_hash($CNFPW,PASSWORD_BCRYPT);

        $emailquery = "SELECT * from sellerreg where email='$E' ";
        $query = mysqli_query($con,$emailquery);

        $emailcount = mysqli_num_rows($query);

        if($emailcount>0){
            echo "Already exists";
        }else{
            if($PW === $CNFPW){
                $sql = "INSERT INTO sellerreg(ID, shopname, gstin, shopaddress, name , email, rdbgender, phone, address, password, cnfpassword, dt) VALUES ('$id','$SHPN', '$gst','$SHPADD','$N','$E','$G','$PH','$ADD','$pass','$cpass',current_timestamp())";

                $iquery = mysqli_query($con, $sql);

            }else{
                echo "passwords are different";
            }   
    }
}        
    mysqli_close($con);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css">
    <link rel="stylesheet" href="user.css">
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Akaya+Telivigala&display=swap" rel="stylesheet">
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Fascinate&display=swap" rel="stylesheet">
    <link rel="preconnect" href="https://fonts.gstatic.com">
    <link href="https://fonts.googleapis.com/css2?family=Lobster&display=swap" rel="stylesheet">
    <link rel="preconnect" href="https://fonts.gstatic.com">
<link href="https://fonts.googleapis.com/css2?family=Texturina:wght@600&display=swap" rel="stylesheet">
<link rel="preconnect" href="https://fonts.gstatic.com">
<link href="https://fonts.googleapis.com/css2?family=Dancing+Script:wght@700&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="user.css">
    <title>Signup Here - Seller</title>
</head>
<body>
    <div class="header" id="header">
        <h1>SWASTH-O-METER</h1>
            <h4>A&nbsp; True&nbsp; Companion&nbsp; Of&nbsp; "ASHA : The Watch"&nbsp;!</h4>
            <h6>"I am here to tell your health status"</h6>
        </div><br><br><br>
        <div class="signupform">
        <form class="signup" action="C:\xampp\htdocs\Interface\thankyouseller.html" method="POST">
                <h1 style="font-family: 'Texturina', serif;">SIGN UP</h1><br><br>
                <div class="shopdetails">
                    <h4 style="color: rgb(189, 222, 252);">Shop Details</h4><br>

                    <label for="shopname"><b>Shop Name</b></label>
                    <input class="shopname" type="shopname" placeholder="&nbsp;&nbsp; Your Shop Name" name="shopname" required><br><br>
                    
                    <label for="gstin"><b>GST_In No.</b></label>
                    <input class="gstin" type="text" placeholder="&nbsp;&nbsp; Enter Shop GSTIN no." name="gstin" required><br><br>
                    
                    <label for="address"><b>Address</b></label>
                    <input class="address" type="text" placeholder="&nbsp;&nbsp; Address" name="shopaddress" required><br><br><br>
                </div>
                <div class="ownerdetail">
                    <h4 style="color: rgb(189, 222, 252);">Owner Details</h4><br>
                    <label for="name"><b>Owner Name</b></label>
                    <input class="name" type="text" placeholder="&nbsp;&nbsp; Your Full Name" name="name" required><br><br>
                    
                    <label for="email"><b>Email</b></label>
                    <input class="email" type="email" placeholder="&nbsp;&nbsp; Your Email" name="email" required><br><br><br>
                    
                    <label for="gender"><b>Gender:&nbsp;&nbsp;
                        <input type="radio" name="rdbgender"id="rdbgender"value="Male"/>&nbsp;Male&nbsp;&nbsp;&nbsp;&nbsp;
                        <input type="radio" name="rdbgender"id="rdbgender"value="Female"/>&nbsp;Female&nbsp;&nbsp;&nbsp;&nbsp;
                        <input type="radio" name="rdbgender"id="rdbgender"value="Other"/>&nbsp;Other&nbsp;&nbsp;&nbsp;&nbsp;<br>
                </label><br><br>
                
                <label for="phone"><b>Mobile</b><br>
                    <input class="tel" type="text" placeholder="&nbsp;Mobile Number" name="phone" pattern="[0-9]{4}[0-9]{2}[0-9]{4}" required></label><br><br>

                    <label for="address"><b>Address</b></label>
                    <input class="address" type="text" placeholder="&nbsp;&nbsp; Address" name="address" required><br><br><br>

                    <label for="psw"><b>Create Password</b></label>
                    <input class="password" type="password" placeholder="&nbsp;&nbsp;Create Your Password" name="password" pattern="(?=.*\d)(?=.*[a-z])(?=.*[A-Z]).{8,}" title="Must contain at least one number and one uppercase and lowercase letters and it should be of at least 8 or more characters" required><br><br>
                    
                    <label for="psw"><b>Confirm-Password</b></label>
                    <input class="password" type="password" placeholder="&nbsp;&nbsp;Confirm Your Password" name="cnfpassword" pattern="(?=.*\d)(?=.*[a-z])(?=.*[A-Z]).{8,}" title="Must contain at least one number and one uppercase and lowercase letters and it should be of at least 8 or more characters" required><br><br>
                </div><br><br>

                <div class="contain">
                    <input type="text" id="capt" style="width: 110px; font-size: 19px; font-weight: bold; background-color: white; color: tomato;" readonly="readonly" >
                    <div id="refresh"><i class="fa fa-refresh " aria-hidden="true"  style="width: 28px; height: 25px; margin-left: 4px;" onclick="cap()"></i></div>

                    <input type="text" id="textinput"  style="width: 104px; margin-left: 45px; background-color: white; color: black;" placeholder="CaptchaHere">
                </div><br><br>
                    
                    <button type="submit" name="submit" class="regbtnseller" onclick="validcap()">Register</button><br><br>
                  <input type="checkbox" checked="checked" name="remember"> Remember me<br><br>
                <label for="signup"><i>Already a member?<a href="http://localhost/interface/sellerrlogin.php" class="signinlink">&nbsp;SignIn</a></i></label>
            </form>
        </div><br><br><br>
        <div class="footer" id="footer">
            <div class="footer1" id="footer1">
                <h6>About</h6><br>
                <p>It is a health wearable that explains the behaviour of our health parameters like body temperature, oxygen concentration and heart rate.<br>This device is also enabled with SOS facility and GPS tracker.</p><br><br><br>
            </div>
            <div class="footer2" id="footer2">
                <h6>-: Contacts :-</h6><br>
                <div class="social-links">
                    <a href="https://www.facebook.com/ankit.jaspal.71" target="_blank"><i class="fa  fa-facebook"></i></a>
                    <a href="" target="_blank"><i class="fa  fa-twitter"></i></a>
                    <a href="" target="_blank"><i class="fa  fa-instagram"></i></a>
                    <a href="" target="_blank"><i class="fa  fa-whatsapp"></i></a>
                    <a href="https://github.com/ROBO-NINJAS" target="_blank"><i class="fa  fa-github"></i></a>
                </div><br><br>
                <div><p>For enquiry : +91 9076635812 (Between 9am to 5pm)</p></div><br><br>
                <div><p>Get In Touch :&nbsp;At &nbsp;U.I.E.T,&nbsp;Kurukshetra</p>
                </div>
            </div>
            <div class="footer3" id="footer3"><br><br>
            <p>Copyrights &#169; Reserved &#64; &#8476;obo &#8511;injaS </p><br>
            <p style="font-family: 'Dancing Script', cursive;">Designed By : TEAM</p>
            </div>
        </div>
        <script type="text/javascript">
                $(document).ready(function(){
                    $(".gstin").change(function(){
                        var gstinput = $(this).val();
                        var gstinformat = new RegExp('^[0-9]{2}[A-Z]{5}[0-9]{4}[A-Z]{1}[1-9 A-Z]{1}Z[0-9 A-Z]{1}$');
                        if(gstinformat.test(gstinput)){
                            return true;
                        }else{
                            alert('Invalid GSTIN');
                            $(".gstin").val('');
                            $(".gstin").focus();
                        }
                    });
                });
            </script>

        <script type="text/javascript">
            function cap() {
        
                var alpha=['A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V'
                           ,'W','X','Y','Z','1','2','3','4','5','6','7','8','9','0','a','b','c','d','e','f','g','h','i',
                           'j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'];
        
                var a=alpha[Math.floor(Math.random()*62)];
                var b=alpha[Math.floor(Math.random()*62)];
                var c=alpha[Math.floor(Math.random()*62)];
                var d=alpha[Math.floor(Math.random()*62)];
                var e=alpha[Math.floor(Math.random()*62)];
                var f=alpha[Math.floor(Math.random()*62)];
        
                var sum=a + b + c + d + e + f;
        
                document.getElementById("capt").value=sum;
            }
        
                 function validcap() {
                var string1 = document.getElementById('capt').value;
                var string2 = document.getElementById('textinput').value;
                if (string1 == string2){
                    alert("Form is validated Succesfully");
                    return true;
                }
                else {
                    alert("Please enter a valid captcha");
                    return false;
                }
            }
        
        
        </script>
</body>
</html>