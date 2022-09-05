#include <DHT.h>  // Including library for dht

#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
String apiKey = "";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "SSID";     // replace with your wifi ssid and wpa2 key
const char *pass =  "PASSWORD";
const char* server = "api.thingspeak.com";

#define DHTPIN 14          //pin where the dht22 is connected

DHT dht(DHTPIN, DHT22);

WiFiClient client;

void setup()
{
  Serial.begin(115200);
  delay(10);
  dht.begin();

  //   initialize LCD
  lcd.begin();
  // turn on LCD backlight
  lcd.backlight();

  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

}

void loop()
{

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (client.connect(server, 80))  //   "184.106.153.149" or api.thingspeak.com
  {

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    /*Serial.print("Temperature: ");
      Serial.print(t);
      Serial.print(" degrees Celcius, Humidity: ");
      Serial.print(h);
      Serial.println("%. Send to Thingspeak.");*/

      lcd.setCursor(0, 0);
      lcd.print(" Temp: ");
      lcd.print(t);
      lcd.print(" C");
      lcd.setCursor(0,1);
      lcd.print(" Humi: ");
      lcd.print(h);
      lcd.print(" %");
  }

  client.stop();

  Serial.println("Waiting...");

  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(1000);
}
