
String request;
int current_status_LED;
int motor_clock;
int motor_anti_clock;
const long utcOffsetInSeconds =19800 ;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String last;



#include <WiFiClient.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


#define ssid "Harshit"
#define pass "Harshit26"
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
            "<!DOCTYPE html>\n"+
"<html lang=\"en\">\n"+
"<head>\n"+
"    <meta charset=\"UTF-8\">\n"+
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"+
"    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"+
"    <title>Document</title>\n"+
"</head>\n"+
"<style>\n"+
"    *{\n"+
"        padding: 0px;\n"+
"        margin:0px;\n"+
"    }\n"+
"    body{\n"+
"        background-color: green;\n"+
"    }\n"+
"\n"+
"    .main{\n"+
"        height: 100vh;\n"+
"        width: 100vw;\n"+
"        \n"+
"        display: grid;\n"+
"        grid-template-rows: 20% 50% 20%;\n"+
"    }\n"+
"\n"+
"    .box{\n"+
"        /* background-color: red; */\n"+
"        margin: 4px;\n"+
"    }\n"+
"\n"+
"    header{\n"+
"        display: flex;\n"+
"        justify-content: center;\n"+
"        align-items: center;\n"+
"        background-color: rgb(233, 202, 233);\n"+
"    }\n"+
"    header h2{\n"+
"        font-size: 38px;\n"+
"        font-weight: bold;\n"+
"        font-family:cursive;\n"+
"        color: rgb(160, 0, 160);\n"+
"        \n"+
"    }\n"+
"\n"+
"    section{\n"+
"        display: flex;\n"+
"        background-color: rgb(173, 226, 226);\n"+
"    }\n"+
"\n"+
"    .switches{\n"+
"        width: 60%;\n"+
"         display: grid; grid-template-rows: 1fr 1fr 1fr;\n"+
"         background-color: rgb(156, 185, 223);\n"+
"       \n"+
"    }\n"+
"    .switches a{\n"+
"        align-self: center;\n"+
"        justify-self: center;\n"+
"    }\n"+
"    \n"+
"    .status{\n"+
"        width: 40%;\n"+
"         display: flex; flex-direction: column;\n"+
"        justify-content: space-around;\n"+
"    }\n"+
"\n"+
"    .leds{\n"+
"        align-self: center;\n"+
"        justify-self: center;\n"+
"        font-size: 26px;\n"+
"        font-weight: bold;\n"+
"        color: blacks;\n"+
"        padding: 8px;\n"+
"        border-radius: 30px;\n"+
"        background-color: red;\n"+
"        color: white;\n"+
"        border-top: 0.5px gray;\n"+
"        outline: none;\n"+
"    }\n"+
"\n"+
"    .time{\n"+
"        font-size: 30px;\n"+
"        padding: 10px;\n"+
"        width: 50%;\n"+
"        align-self: center;\n"+
"        justify-self: center;\n"+
"        border-radius:40px;\n"+
"        border-top: 1px gray;\n"+
"        outline: none;\n"+
"    }\n"+
"\n"+
"    .area{\n"+
"        font-size: 18px;\n"+
"        padding: 7px;\n"+
"        width: 50%;\n"+
"        align-self: center;\n"+
"        justify-self: center;\n"+
"        border-radius:20px;\n"+
"        border-top: 1px gray;\n"+
"        outline: none;\n"+
"    }\n"+
"\n"+
"</style>\n"+
"\n"+
"<body>\n"+
"    <div class=\"main\">\n"+
"        <header  class=\"box\">\n"+
"            <h2>Control your Home Appliance</h2>\n"+
"        </header >\n"+
"        <section class=\"box\">\n"+
"            <div class=\"switches\">\n"+
"                <a href=\"LED\" ><input class=\"leds\" type=\"button\" value=\"Led1\"></a>\n"+
"                <a href=\"MOTOR_CLOCKWISE\" ><input class=\"leds\" type=\"button\" value=\"MOTOR_CLOCKWISE\"></a>\n"+
"                <a href=\"MOTOR_ANTI_CLOCKWISE\" ><input class=\"leds\" type=\"button\" value=\"MOTOR_ANTI_CLOCKWISE\"></a>\n"+
"                \n"+
"            </div>\n"+
"            <div class=\"status\">\n"+
"                 <h2 style=\"font-size: 38px;font-weight: bold;font-family:cursive;color: rgb(160, 0, 160);\" class=\"time\">\n"+ daysOfTheWeek[timeClient.getDay()]+ " " +timeClient.getHours()+":"+ timeClient.getMinutes()+":" + timeClient.getSeconds() +"</h2>\n"+
"                 <h2 style=\"margin: 3px; font-size: 38px;font-weight: bold;font-family:cursive;color: rgb(160, 0, 160);\" class=\"area\">\n"+ last +"</h2>\n"+
"            </div>\n"+
"        </section>\n"+
"        <footer class=\"box\">\n"+
"            <h2>this is foot</h2>\n"+
"        </footer>\n"+
"    </div>\n"+
"</body>\n"+
"</html>"+
            "\r\n";
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
  
  if(current_status_LED==0 && request.indexOf("/LED")!= -1)
  {
    digitalWrite(D0,1);
    last = "D0 is high";
    Serial.println(last );
  }
  else if (current_status_LED==1 && request.indexOf("/LED")!= -1)
  {
    digitalWrite(D0,0);
    last="D0 is low";
    Serial.println(last );
  }
  if(motor_clock==0 && request.indexOf("/MOTOR_CLOCKWISE")!= -1)
  {
    digitalWrite(D1,1);
    digitalWrite(D2,0);
    last="Motor is clockwise direction";
    //Serial.println("D1 is high" );
    Serial.println(last);
    
  }
  else if (motor_clock==1 && request.indexOf("/MOTOR_CLOCKWISE")!= -1)
  {
    digitalWrite(D1,0);
    digitalWrite(D2,0);
    last="Motor is oFF";
    Serial.println(last);
    //Serial.println("D1 is low" );
  }
  if(motor_anti_clock==0 && request.indexOf("/MOTOR_ANTI_CLOCKWISE")!= -1)
  {
    digitalWrite(D2,1);
    digitalWrite(D1,0);
    //Serial.println("D1 is high" );
    last="Motor is anticlockwise direction";
    Serial.println(last);
    
  }
  else if (motor_anti_clock==1 && request.indexOf("/MOTOR_ANTI_CLOCKWISE")!= -1)
  {
    digitalWrite(D2,0);
    digitalWrite(D1,0);
    last="Motor is off";
    Serial.println(last);
    //Serial.println("D1 is low" );
  }
  timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());

// 
//  client.print("<html>");
//  client.print("<head>");
//  client.print("<title>Dashboard</title>");
//  client.print("</head>");
//  client.print("<body>");
//  client.print("<a href='LED'><input type='button' value='LED'></a>");
//  client.print("<a href='MOTOR_CLOCKWISE'><input type='button' value='MOTOR_CLOCKWISE'></a>");
//  client.print("<a href='MOTOR_ANTI_CLOCKWISE'><input type='button' value='MOTOR_ANTI_CLOCKWISE'></a>");
// client.print("<h1>");
//  client.print(last);
//  client.print(daysOfTheWeek[timeClient.getDay()]);
//  client.print(" ");
//  client.print(timeClient.getHours());
//  client.print(":");
//  client.print(timeClient.getMinutes());
//  client.print(":");
//  client.print(timeClient.getSeconds());  
//  client.print("</h1>");
//  //client.print("<a href='LED-OFF'><input type='button' value='LED-OFF'></a>");
//  client.print("</body>");
//  client.print("</html>");

client.println(prepareHtmlPage());
}
