String request;
int current_status_LED;
int motor_clock;
int motor_anti_clock;
const long utcOffsetInSeconds = 19800;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String last;
String led_switch_color;
int pir_status;
String s = "led-on";
String motor_c = "c-on";
String motor_ac = "ac-on";


#include <WiFiClient.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


#define ssid "kamal pc"
#define pass "123456789"
WiFiServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);






void setup() {
  Serial.begin(115200);
  Serial.print("Connecting WiFi");
  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.print("Connected to the network");
  server.begin();
  Serial.println(WiFi.localIP());
  pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);//for LCD display
  pinMode(D4,OUTPUT);
  pinMode(D5,INPUT); // for pir input
  pinMode(D6,OUTPUT); //pir led
  pinMode(D7,OUTPUT); // for buzzer

  digitalWrite(D6,LOW); // initially pir led is low
  
  timeClient.begin();
}


String prepareHtmlPage()
{
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE html>\n"
"<html lang=\"en\">\n"
"<head>\n"
"    <meta charset=\"UTF-8\">\n"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"
"    <title>Document</title>\n"
"</head>\n"
"<style>\n"
"    *{\n"
"        padding: 0px;\n"
"        margin:0px;\n"
"    }\n"
"    body{\n"
"        background-color: green;\n"
"    }\n"
"\n"
"    .main{\n"
"        height: 100vh;\n"
"        width: 100vw;\n"
"        \n"
"        display: grid;\n"
"        grid-template-rows: 20% 50% 30%;\n"
"    }\n"
"\n"
"    .box{\n"
"        /* background-color: red; */\n"
"        margin: 4px;\n"
"    }\n"
"\n"
"    header{\n"
"        display: flex;\n"
"        justify-content: center;\n"
"        align-items: center;\n"
"        background-color: rgb(233, 202, 233);\n"
"    }\n"
"    header h2{\n"
"        font-size: 38px;font-weight: bold;font-family:cursive;color: rgb(160, 0, 160);\n"
"        \n"
"    }\n"
"\n"
"    section{\n"
"        display: flex;\n"
"        background-color: rgb(173, 226, 226);\n"
"    }\n"
"\n"
"    .switches{\n"
"        width: 60%;\n"
"         display: grid; grid-template-rows: 1fr 1fr 1fr;\n"
"         background-color: rgb(156, 185, 223);\n"
"       \n"
"    }\n"
"    .switches a{\n"
"        align-self: center;\n"
"        justify-self: center;\n"
"    }\n"
"    \n"
"    .status{\n"
"        width: 40%;\n"
"         display: flex; flex-direction: column;\n"
"        justify-content: space-around;\n"
"    }\n"
"\n"
"\n"
"    .leds{\n"
"        align-self: center;\n"
"        justify-self: center;\n"
"        font-size: 26px;\n"
"        font-weight: bold;\n"
"        color: blacks;\n"
"        padding: 8px;\n"
"        border-radius: 30px;\n"
"        background-color: red;\n"
"        color: white;\n"
"        border-top: 0.5px gray;\n"
"        outline: none;\n"
"    }\n"
"\n"
"    .time{\n"
"        font-size: 30px;\n"
"        padding: 10px;\n"
"        width: 50%;\n"
"        align-self: center;\n"
"        justify-self: center;\n"
"        border-radius:40px;\n"
"        border-top: 1px gray;\n"
"        outline: none;\n"
"    }\n"
"\n"
"    .area{\n"
"        font-size: 18px;\n"
"        padding: 7px;\n"
"        width: 50%;\n"
"        align-self: center;\n"
"        justify-self: center;\n"
"        border-radius:20px;\n"
"        border-top: 1px gray;\n"
"        outline: none;\n"
"    }\n"
"\n"
"    footer{\n"
"        display: flex;\n"
"        align-items: center;\n"
"        justify-content: center;\n"
"        background-color: rgb(171, 128, 190);\n"
"    }\n"
"\n"
"    .form_c{\n"
"        /* border: 1px solid black; */\n"
"        width: 300px;\n"
"        margin-top: 15px;\n"
"        height: 60px;\n"
"        display: flex;\n"
"        justify-content: space-between;\n"
"    }\n"
"\n"
"    .alarm{\n"
"        height: 90%;\n"
"        width: 60%;\n"
"        font-size: 36px;\n"
"        border: none;\n"
"        outline: none;\n"
"        border-radius: 30px;\n"
"        color: white;\n"
"        background-color: rgb(248, 52, 189);\n"
"    } \n"
"\n"
"    .a_b{\n"
"        height: 90%;\n"
"        width: 30%;\n"
"        font-size: 20px;\n"
"        border: none;\n"
"        outline: none;\n"
"        border-radius: 30px;\n"
"        color: white;\n"
"        background-color: rgb(248, 52, 189);\n"
"    }\n"
"\n"
"\n"
"</style>\n"
"\n"
"<body>\n"
"    <div class=\"main\">\n"
"        <header  class=\"box\">\n"
"            <h2>Control your Home Appliance</h2>\n"
"        </header >\n"
"        <section class=\"box\">\n"
"            <div class=\"switches\">\n"
"<a href=\"\n"
+ s +
"\">"
"<input class=\"leds\" type=\"button\" value=\"Led1\" style=\"background-color:"+ led_switch_color+ ";\" ></a>\n"+
"<a href=\"\n"
+ motor_c +
"\">"
"<input class=\"leds\" type=\"button\" value=\"MOTOR_CLOCKWISE\"></a>\n"
"<a href=\"\n"
+ motor_ac +
"\">"
"<input class=\"leds\" type=\"button\" value=\"MOTOR_ANTI_CLOCKWISE\"></a>\n"
"                \n"
"            </div>\n"
"            <div class=\"status\">\n"
"                 <h2 style=\"font-size: 38px;font-weight: bold;font-family:cursive;color: rgb(160, 0, 160);\" class=\"time\">\n"+ daysOfTheWeek[timeClient.getDay()] + " " + timeClient.getHours()+":"+ timeClient.getMinutes( ) +":" + timeClient.getSeconds() +"</h2>\n"+
"                 <h2 style=\"margin: 3px; font-size: 38px;font-weight: bold;font-family:cursive;color: rgb(160, 0, 160);\" class=\"area\">\n"+ last +"</h2>\n"+
"            </div>\n"
"        </section>\n"
"        <footer class=\"box\" style=\"\" >\n"
"            <form action=\"\" method=\"GET\" class=\"form_c\">\n"
"                <input type=\"time\" name=\"time\" class=\"alarm\">\n"
"                <input type=\"submit\" name=\"\" id=\"\" value=\"set alarm\" class=\"a_b\" >\n"
"            </form>\n"
"        </footer>\n"
"    </div>\n"
"</body>\n"
"</html>" ;
  return htmlPage;
}










void loop() {
  WiFiClient client =server.available();
  if(!client)
  {
    return;
  }
  current_status_LED = digitalRead(D0);
  request=client.readStringUntil('/r');
  motor_clock= digitalRead(D1);
  motor_anti_clock= digitalRead(D2);
  
  if(current_status_LED==0 && request.indexOf("/led-on")!= -1)
  {
    digitalWrite(D0,1);
    last = "D0 is high";
    led_switch_color = "green";
    s = "led-off";
    Serial.println(last );
  }
  else if (current_status_LED==1 && request.indexOf("/led-off")!= -1)
  {
    digitalWrite(D0,0);
    last="D0 is low";
    led_switch_color = "red";
    s = "led-on"; 
    Serial.println(last );
  }
  if(motor_clock==0 && request.indexOf("/c-on")!= -1)
  {
    digitalWrite(D1,1);
    digitalWrite(D2,0);
    motor_c = "c-off";
    last="motor is on in c";
   
    //Serial.println("D1 is high" );
    Serial.println(last);
    
  }
  else if (motor_clock==1 && request.indexOf("/c-off")!= -1)
  {
    digitalWrite(D1,0);
    digitalWrite(D2,0);
    last="Motor is oFF";
    motor_c = "c-on";
    Serial.println(last);
    //Serial.println("D1 is low" );
  }
  if(motor_anti_clock==0 && request.indexOf("/ac-on")!= -1)
  {
    digitalWrite(D2,1);
    digitalWrite(D1,0);
    //Serial.println("D1 is high" );
    last="Motor is anticlockwise direction";
    motor_ac = "ac-off";
    Serial.println(last);
    
  }
  else if (motor_anti_clock==1 && request.indexOf("/ac-off")!= -1)
  {
    digitalWrite(D2,0);
    digitalWrite(D1,0);
    last="Motor is off";
    motor_ac = "ac-on";
    Serial.println(last);
    //Serial.println("D1 is low" );
  }

  pir_status = digitalRead(D5);

  if(pir_status){
    digitalWrite(D6,HIGH);
    Serial.println("pir led is on");
    
  } else{
    digitalWrite(D6,LOW);
    Serial.println("pir led is off");
  }
  


  
  timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());



client.println(prepareHtmlPage());
}
