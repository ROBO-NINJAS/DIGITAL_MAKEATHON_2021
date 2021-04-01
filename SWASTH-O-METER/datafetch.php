<?php 
session_start();

?>


<html>
     <head>
         <title>DETAILS</title>
         <style>
           .header{
             width: 100%;
             height: 140px;
             background: linear-gradient(rgb(127, 208, 255),rgb(255, 132, 255));
             }
             .logoutbtn{
               display: flex;
               flex-direction: column;
               margin: auto;
               border: 2px solid rgb(255,132,265);
               border-radius:4px;
               text-align:center;
               justify-content:center;
               align-items:center;
               width:55px;
               height:25px;
               background-color:rgb(255, 132, 255);
               color:black;
               margin-right:16px;
               margin-top:-50px;
             }
             .logoutbtn a{
                 text-decoration:none;
                 font-weight:bolder;
             }
             .logoutbtn:hover{
                background-color:rgb(127, 208, 255);
             }
         </style>
     </head>
     <body>
       <div class="header" id="header">
        <h1 style="text-align: center;">HeLLo!&nbsp;<?php echo $_SESSION['name']; ?></h1>
        <div class="logoutbtn" id="logoutbtn"><a href='logout.php'>LogOut</a></div>
       </div>
       <script>

       </script>
         <table border="2" cellspacing="0" width="640" height="480" bordercolor="#0000FF">
                    <tr>
                            <td colspan="4"><h1 align="center" color="#00FFFF">Data Collection Dashboard</h1></td>
                  </tr>
                  <tr>
                    <td><iframe width="530" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/charts/2?bgcolor=FFEBCD&color=556B2F&dynamic=true&results=15&title=TEMPERATURE&type=spline&xaxis=DAYSTAMP&yaxis=TEMPERATURE"></iframe></td>
                    <td><iframe width="500" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/widgets/288481"></iframe></td>
                    <td><iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/widgets/288474"></iframe></td>
          </tr>
          <tr>
            <td><iframe width="530" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/charts/3?bgcolor=FFF0F5&color=8B8386&dynamic=true&results=15&title=PULSE+-+METER&type=spline&xaxis=DAYSTAMP&yaxis=PULSE"></iframe></td>
            <td><iframe width="500" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/widgets/288521"></iframe></td>
            <td><iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/widgets/288522"></iframe></td>
         </tr>
         <tr>
            <td><iframe width="530" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/charts/4?bgcolor=C1FFC1&color=698B69&dynamic=true&results=15&title=SpO2+LEVELS&type=spline&xaxis=DAYSTAMP&yaxis=SPO2"></iframe></td>
            <td><iframe width="500" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/widgets/288525"></iframe></td>
            <td><iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/1341255/widgets/288523"></iframe></td>
         </tr>

         </table>
     </body>
</html>