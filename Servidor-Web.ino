#include <WiFi.h>
// const char* ssid = "IME-Servicos";
// const char* password = "";
const char* ssid = "RedeTeste";
const char* password = "Testeapenas";

WiFiServer server(80);

void setup()
{
Serial.begin(115200);
pinMode(5, OUTPUT); // modo pino 5
delay(10);
Serial.println(); Serial.println();
Serial.print("Conectando com "); // Inicia conexão Wifi
Serial.println(ssid);

WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi conectado.");
Serial.println("Endereco IP: ");
Serial.println(WiFi.localIP());
server.begin();
}

int value = 0;

void loop(){
WiFiClient client = server.available(); // listen for incoming clients
if (client) { // if you get a client,
Serial.println("Novo Cliente."); // print a message out the serial port
String currentLine = ""; // make a String to hold incoming data from the client
while (client.connected()) { // loop while the client's connected
if (client.available()) { // if there's bytes to read from the client,
char c = client.read(); // read a byte, then
Serial.write(c); // print it out the serial monitor
if (c == '\n') { // if the byte is a newline character
// if the current line is blank, you got two newline characters in a row.
// that's the end of the client HTTP request, so send a response:
if (currentLine.length() == 0) {

  const char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html lang="en">

<head class="head">
    <title>Servidor Web Debug</title>

    <style>
    body{
        display: flex;
        flex-direction: column;
        align-items: center;
        font-family: Arial, Helvetica, sans-serif;
    }

    .title{
        display: flex;
        justify-content: center;
        align-items: center;
        width: 100%;
        height: 70px;
        background-color: rgb(34, 34, 34);
        box-shadow: 0 0 10px black;
    }

    .title h1{
        font-size: 35px;
        font-weight: lighter;
        color: white;
    }

    .clock {
        position: absolute;
        padding: 10px;
        right: 0;
        bottom: 0;
        font-size: 30px;
        font-family: Orbitron;
        letter-spacing: 7px;
    }

    .view{
        width: 100%;
        display: flex;
        flex-direction: row;
        justify-content: space-evenly;
        align-items: center;
    }

    .table{
        border-collapse: collapse;
        flex-direction: column;
        width: 20%;
        margin: 25px 0;
        font-size: 1.2em;
        border-radius: 5px 5px 0 0;
        overflow: hidden;
        box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);
    }

    .table thead tr{
        font-size: 150%;
        background-color: #009879;
        color: white;
    }

    .table th{
        padding: 10px 0;
    }

    .table tbody tr{
        border-bottom: 1px solid #dddddd;
    }

    .table tbody tr:nth-of-type(even){
        background-color: #f9f9f9;
    }

    .table tbody tr:last-of-type{
        border-bottom: 1px solid #009879;
    }

    .table td{
        font-size: .9em;
        padding: 12px 15px;
    }
    </style>
</head>

<body>
    <div class="title">
        <h1>Interface para medicao de sensores</h1>
    </div>

    <div class="view">
        <table class="table">
            <thead>
                <tr>
                    <th>Sensor de luminosidade</th>
                </tr>
            </thead>

            <tbody>
                <tr>
                    <td>Luz: <span id="luz_ligada"></span></td>
                </tr>

                <tr>
                    <td>Luminosidade: <span id="luminosidade"></span></td>
                </tr>
            </tbody>
        </table>

        <table class="table">
            <thead>
                <tr>
                    <th>Sensor de umidade</th>
                </tr>
            </thead>

            <tbody>
                <tr>
                    <td>Ar condicionado: <span id="ar_condicionado"></span></td>
                </tr>

                <tr>
                    <td>Temperatura: <span id="temperatura"></span></td>
                </tr>
            </tbody>
        </table>

        <table class="table">
            <thead>
                <tr>
                    <th>Camera</th>
                </tr>
            </thead>

            <tbody>
                <tr>
                    <td/>
                </tr>

                <tr>
                    <td><span id="camera"></span></tr>
                </tr>

                <tr>
                    <td/>
                </tr>
            </tbody>
        </table>
    </div>
    <script>
        function loadDoc(){
            loadTempe();
            // loadHumid();
            loadLumin();
            loadCam();
            // loadProx();
            setTimeout(loadDoc, 30000);
        };

        function loadLumin(){
            const xhttp = new XMLHttpRequest();
            xhttp.onload = function(){
                luzLigada = "Apagada";
                luminosidade = this.responseText

                if (luminosidade > 1000) {luzLigada = "Acesa";}

                document.getElementById("luz_ligada").innerHTML = luzLigada;
                document.getElementById("luminosidade").innerHTML = luminosidade;
            };
            xhttp.open("GET", "http://192.168.22.56", true);
            xhttp.send();
        };

        function loadTempe(){
            const xhttp = new XMLHttpRequest();
            xhttp.onload = function(){
                arLigado = "Ligado";
                temperatura = this.responseText

                if (temperatura > 23) {arLigado = "Desligado";}

                document.getElementById("ar_condicionado").innerHTML = arLigado;
                document.getElementById("temperatura").innerHTML = temperatura;
            };
            xhttp.open("GET", "http://192.168.22.231", true);
            xhttp.send();
        };

        function loadCam(){
            const xhttp = new XMLHttpRequest();
            xhttp.onload = function(){
                document.getElementById("camera").innerHTML = this.responseText;
            };
            xhttp.open("GET", "http://192.168.43.122", true);
            xhttp.send();
        };
        loadDoc();
    </script>

    <div id="MyClockDisplay" class="clock" onload="showTime()"></div>
    <script>
        function showTime(){
            var date = new Date();
            var h = date.getHours(); // 0 - 23
            var m = date.getMinutes(); // 0 - 59
            var s = date.getSeconds(); // 0 - 59
            var session = "AM";
            if(h == 0) {h = 12;}
            if(h > 12) {h = h - 12; session = "PM";}
            h = (h < 10) ? "0" + h : h;
            m = (m < 10) ? "0" + m : m;
            s = (s < 10) ? "0" + s : s;
            var time = h + ":" + m + ":" + s + " " + session;
            document.getElementById("MyClockDisplay").innerText = time;
            document.getElementById("MyClockDisplay").textContent = time;
            setTimeout(showTime, 1000);
        };
        showTime();
    </script>
</body>
</html>

)rawliteral";

//client.println("<!DOCTYPE html><html lang=\"en\"><head class=\"head\"> <title>Servidor Web Debug</title> <style>body{display: flex; flex-direction: column; align-items: center; font-family: Arial, Helvetica, sans-serif;}.title{display: flex; justify-content: center; align-items: center; width: 100%; height: 70px; background-color: rgb(34, 34, 34); box-shadow: 0 0 10px black;}.title h1{font-size: 35px; font-weight: lighter; color: white;}.clock{position: absolute; padding: 10px; right: 0; bottom: 0; font-size: 30px; font-family: Orbitron; letter-spacing: 7px;}.view{width: 100%; display: flex; flex-direction: row; justify-content: space-evenly; align-items: center;}.table{border-collapse: collapse; flex-direction: column; width: 20%; margin: 25px 0; font-size: 1.2em; border-radius: 5px 5px 0 0; overflow: hidden; box-shadow: 0 0 20px rgba(0, 0, 0, 0.15);}.table thead tr{font-size: 150%; background-color: #009879; color: white;}.table th{padding: 10px 0;}.table tbody tr{border-bottom: 1px solid #dddddd;}.table tbody tr:nth-of-type(even){background-color: #f9f9f9;}.table tbody tr:last-of-type{border-bottom: 1px solid #009879;}.table td{font-size: .9em; padding: 12px 15px;}</style></head><body> <div class=\"title\"> <h1>Interface para medicao de sensores</h1> </div><div class=\"view\"> <table class=\"table\"> <thead> <tr> <th>Sensor de luminosidade</th> </tr></thead> <tbody> <tr> <td>Luz: <span id=\"luz_ligada\"></span></td></tr><tr> <td>Luminosidade: <span id=\"luminosidade\"></span></td></tr></tbody> </table> <table class=\"table\"> <thead> <tr> <th>Sensor de umidade</th> </tr></thead> <tbody> <tr> <td>Ar condicionado: <span id=\"ar_condicionado\"></span></td></tr><tr> <td>Temperatura: <span id=\"temperatura\"></span></td></tr></tbody> </table> <table class=\"table\"> <thead> <tr> <th>Camera</th> </tr></thead> <tbody> <tr> <td/> </tr><tr> <td><span id=\"camera\"></span></tr></tr><tr> <td/> </tr></tbody> </table> </div><script>function loadDoc(){loadTempe(); // loadHumid(); loadLumin(); loadCam(); // loadProx(); setTimeout(loadDoc, 5000);}; function loadLumin(){const xhttp=new XMLHttpRequest(); xhttp.onload=function(){luzLigada=\"Apagada\"; luminosidade=this.responseText if (luminosidade > 1000){luzLigada=\"Acesa\";}document.getElementById(\"luz_ligada\").innerHTML=luzLigada; document.getElementById(\"luminosidade\").innerHTML=luminosidade;}; xhttp.open(\"GET\", \"http://192.168.22.56\", true); xhttp.send();}; function loadTempe(){const xhttp=new XMLHttpRequest(); xhttp.onload=function(){arLigado=\"Ligado\"; temperatura=this.responseText if (temperatura > 23){arLigado=\"Desligado\";}document.getElementById(\"ar_condicionado\").innerHTML=arLigado; document.getElementById(\"temperatura\").innerHTML=temperatura;}; xhttp.open(\"GET\", \"http://192.168.22.231\", true); xhttp.send();}; function loadCam(){const xhttp=new XMLHttpRequest(); xhttp.onload=function(){document.getElementById(\"camera\").innerHTML=this.responseText;}; xhttp.open(\"GET\", \"http://192.168.43.122\", true); xhttp.send();}; loadDoc(); </script> <div id=\"MyClockDisplay\" class=\"clock\" onload=\"showTime()\"></div><script>function showTime(){var date=new Date(); var h=date.getHours(); // 0 - 23 var m=date.getMinutes(); // 0 - 59 var s=date.getSeconds(); // 0 - 59 var session=\"AM\"; if(h==0){h=12;}if(h > 12){h=h - 12; session=\"PM\";}h=(h < 10) ? \"0\" + h : h; m=(m < 10) ? \"0\" + m : m; s=(s < 10) ? \"0\" + s : s; var time=h + \":\" + m + \":\" + s + \" \" + session; document.getElementById(\"MyClockDisplay\").innerText=time; document.getElementById(\"MyClockDisplay\").textContent=time; setTimeout(showTime, 1000);}; showTime(); </script></body></html>");
client.println(index_html);

break;
} else { // if you got a newline, then clear currentLine:
currentLine = "";
}
} else if (c != '\r') { // if you got anything else but a carriage return character,
currentLine += c; // add it to the end of the currentLine
}
// Verifica se o cliente pediu "GET /H" ou "GET /L"
if (currentLine.endsWith("GET /H")) {
digitalWrite(5, HIGH); // GET /H turns the LED on
}
if (currentLine.endsWith("GET /L")) {
digitalWrite(5, LOW); // GET /L turns the LED off
}
}
}
client.stop(); // fecha conexão
Serial.println("Cliente desconectado.");
}
} // final do loop()
