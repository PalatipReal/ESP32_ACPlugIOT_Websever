const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">;
<title>AC Plug IOT</title>
<style>
  html{ 
    font-family: Helvetica; 
    display: inline-block; 
    margin: 0px auto; 
    text-align: center;
      }
  body{
    margin-top: 50px;
      } 
    h1{
      color: #444444;
      margin: 5px auto 30px;
      } 
     h3{
      color: #444444;
      margin-bottom: 50px;
       }
       h4{
         color: #444444;
       }
p {
  font-size: 14px;
  color: #888;
  margin-bottom: 10px;
  }

  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }
#border {
  border: 5px outset #1C6EA4;
  border-radius: 25px 25px 25px 25px;
  margin-top: 10px;
}
#border2 {
  border: 3px outset  yellowgreen;
  margin: 25px;
}
</style>
</head>
<body>
<div id="demo">
  <h1>ESP32 AC Plug IOT</h1>

  <div id="border">
    <h2> Relay 1</h2>
    <button  type="button" onclick="sendData1(1)">LED ON</button>
    <button  type="button" onclick="sendData1(0)">LED OFF</button><BR>
    <h3>Relay 1 State is : <span id="LEDState1">NA</span> </h3><br>

    <div id="border2">
      <h3>
        Timer1 : 
        <h4> [On]
          Hour: <span id="Get_Timer1_On_Hour"/>
            Min : <span id="Get_Timer1_On_Min"/>
            Sec : <span id="Get_Timer1_On_Sec"/>
        </h4>
        <h4> [Off] 
          Hour: <span id="Get_Timer1_Off_Hour"/>
            Min : <span id="Get_Timer1_Off_Min"/>
            Sec : <span id="Get_Timer1_Off_Sec"/>
        </h4>
        <button  type="button" >Reset</button>
        <div>
          <h3> Set Timer1 On </h3>
          <input type="time" id="timer1"/> 
          <button  
          type="button" 
          onclick="SendTimer1()">
          Set
        </button>
        </div>
        <div>
          <h3> Set Timer1 Off </h3>
          <input type="text" name=""/> 
          <input type="text" name=""/> 
          <input type="text" name=""/>
          <button  type="button" >Set</button>
        </div>
      </h3>
    </div>
  </div>

</div>
<div id="border" >
  <h2> Relay 2</h2>
  <button  type="button" onclick="sendData2(1)">LED ON</button>
  <button  type="button" onclick="sendData2(0)">LED OFF</button><BR>
  <h3>Relay 2 State is : <span id="LEDState2">NA</span> </h3>

    <div id="border2">
      <h3>
        Timer2 : 
        <h4> [On]
          Hour: <span id="Get_Timer2_On_Hour"/>
            Min : <span id="Get_Timer2_On_Min"/>
            Sec : <span id="Get_Timer2_On_Sec"/>
        </h4>
        <h4> [Off] 
          Hour: <span id="Get_Timer2_Off_Hour"/>
            Min : <span id="Get_Timer2_Off_Min"/>
            Sec : <span id="Get_Timer2_Off_Sec"/>
        </h4>
        <button  type="button" >Reset</button>
        <div>
          <h3> Set Timer2 On </h3>
          <input type="text" name=""/> 
          <input type="text" name=""/> 
          <input type="text" name=""/>
          <button  type="button" >Set</button>
        </div>
        <div>
          <h3> Set Timer2 Off </h3>
          <input type="text" name=""/> 
          <input type="text" name=""/> 
          <input type="text" name=""/>
          <button  type="button" >Set</button>
        </div>
      </h3>
    </div>

</div>
<div>
  <br>
  ADC Value is : <span id="ADCValue">0</span><br>
   
</div>
<div>
 <div style="text-align:center;"><b></b><br>Real Time Data Logging with Graphs on ESP32</div>
    <div class="chart-container" position: relative; height:350px; width:100%">
        <canvas id="Chart" width="400" height="400"></canvas>
    </div>
<br>
<br>  

<script>
  function SendTimer1() {
    var Timer1 = document.getElementById("timer1").value;
    console.log(Timer1);
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET","setTimer1",true);
    xhttp.send(Timer1);
  }
  </script>

<script>
//Graphs visit: https://www.chartjs.org
var values = [];
var timeStamp = [];
function showGraph(){
    for (i = 0; i < arguments.length; i++) {
      values.push(arguments[i]);    
    }

    var ctx = document.getElementById("Chart").getContext('2d');
    var Chart2 = new Chart(ctx, {
        type: 'line',
        data: {
            labels: timeStamp,  //Bottom Labeling
            datasets: [{
                label: "Cerrent",
                fill: false,  //Try with true
                backgroundColor: 'rgba( 243, 156, 18 , 1)', //Dot marker color
                borderColor: 'rgba( 243, 156, 18 , 1)', //Graph Line Color
                data: values,
            }],
        },
        options: {
            title: {
                    display: true,
                    text: "ADC Voltage"
                },
            maintainAspectRatio: false,
            elements: {
            line: {
                    tension: 0.5 //Smoothening (Curved) of data lines
                }
            },
            scales: {
                    yAxes: [{
                        ticks: {
                            beginAtZero:true
                        }
                    }]
            }
        }
    });
}
//On Page load show graphs
window.onload = function() {
  console.log(new Date().toLocaleTimeString());
  showGraph(5,10,4,58);
  getLED1();
  setTimeout(getLED2,7000);
};

</script>
</div>

<script src = "https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.min.js"></script>  

<script>
function sendData1(led) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("LEDState1").innerHTML =
        this.responseText;
      }
    };
    xhttp.open("GET","setLED1?LEDstate1="+led, true);
    xhttp.send();
}
function sendData2(led) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("LEDState2").innerHTML =
        this.responseText;
      }
    };
    xhttp.open("GET","setLED2?LEDstate2="+led, true);
    xhttp.send();
}
setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 5000); //2000mSeconds update rate

function getLED1() {
  console.log("Get LED1 From DB");
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    console.log("LED1 readyState: "+this.readyState+" Status: "+this.status)
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState1").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "getLED1", true);
  xhttp.send();
}

function getLED2() {
  console.log("Get LED2 From DB");
  var LED2 = new XMLHttpRequest();
  LED2.onreadystatechange = function() {
    console.log("LED2 readyState: "+this.readyState+" Status: "+this.status)
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState2").innerHTML = this.responseText;
    }
  };
  LED2.open("GET", "getLED2", true);
  LED2.send();
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ADCValue").innerHTML = this.responseText;
      var time = new Date().toLocaleTimeString();
      var ADCValue = this.responseText; 
      values.push(ADCValue);
      timeStamp.push(time);
      showGraph();  //Update Graphs
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}
</script>

</body>
</html>
)=====";
