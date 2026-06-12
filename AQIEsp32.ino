#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <SensirionI2CSen5x.h>
#include <TFT_eSPI.h>

/* ===== WIFI ===== */
const char* ssid = "SEN54_MONITOR";
const char* password = "12345678";

WebServer server(80);

/* ===== OBJECTS ===== */
SensirionI2CSen5x sen5x;
TFT_eSPI tft = TFT_eSPI();

/* ===== SEN54 VARIABLES ===== */
float pm1, pm2_5, pm4, pm10;
float humidity, temperature;
float vocIndex, noxIndex;
#define RELAY_PIN   13
#define BUZZER_PIN  12

#define PM_LIMIT    4000   // 4000 ug/m3 (raw SEN54 value)
bool alarmStatus = false;
bool alarm = false;

/* ===== WEB PAGE ===== */
void handleRoot() 
{
  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv="refresh" content="2">
<title>SEN54 Air Quality</title>

<style>
body{
  font-family: Arial;
  background:#0f172a;
  color:white;
  text-align:center;
  margin:0;
}
h1{color:#38bdf8}
.card{
  background:#1e293b;
  margin:12px;
  padding:15px;
  border-radius:12px;
  box-shadow:0 0 10px #000;
}
.value{font-size:22px;font-weight:bold}
.good{color:#22c55e}
.bad{color:#ef4444}
.warn{color:#facc15}

.status{
  font-size:24px;
  font-weight:bold;
  padding:12px;
  margin:15px;
  border-radius:10px;
}
.normal{background:#14532d}
.alarm{background:#7f1d1d}
</style>
</head>

<body>

<h1>SEN54 Air Quality Monitor</h1>
)rawliteral";

  /* ===== STATUS BANNER ===== */
if (alarmStatus) {
  page += "<div class='status alarm'>AIR QUALITY ALARM</div>";
} else {
  page += "<div class='status normal'>AIR QUALITY NORMAL</div>";
}


  /* ===== PM VALUES ===== */
  page += "<div class='card'><div>PM1.0</div><div class='value'>" + String(pm1/100,1) + "µg/m³</div></div>";
  page += "<div class='card'><div>PM2.5</div><div class='value'>" + String(pm2_5/100,1) + "µg/m³</div></div>";
  page += "<div class='card'><div>PM4.0</div><div class='value'>" + String(pm4/100,1) + "µg/m³</div></div>";
  page += "<div class='card'><div>PM10</div><div class='value'>" + String(pm10/100,1) + "µg/m³</div></div>";

  /* ===== ENV VALUES ===== */
  page += "<div class='card'>Temperature : <b>" + String(temperature,1) + " °C</b></div>";
  page += "<div class='card'>Humidity : <b>" + String(humidity,1) + " %</b></div>";
  page += "<div class='card'>VOC Index : <b>" + String(vocIndex,0) + "</b></div>";

  /* ===== RELAY + BUZZER ===== */
  page += "<div class='card'>";
  page += "<h3>System Status</h3>";

  if (alarmStatus) {
    page += "<p class='bad'>Relay : TRIPPED</p>";
    page += "<p class='bad'>Buzzer : ON</p>";
  } else {
    page += "<p class='good'>Relay : ON</p>";
    page += "<p class='good'>Buzzer : OFF</p>";
  }

  page += "</div>";

  page += "</body></html>";

  server.send(200, "text/html", page);
}


/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);   // Relay ON (NORMAL)
  digitalWrite(BUZZER_PIN, LOW);   // Buzzer OFF

  /* I2C */
  Wire.begin(21, 22);
  sen5x.begin(Wire);

  /* SEN54 INIT */
  sen5x.deviceReset();
  sen5x.startMeasurement();

  /* TFT INIT */
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, 5);
  tft.println("SEN54 AIR QUALITY");
  tft.drawFastHLine(0, 30, 320, TFT_BLUE);

  /* WIFI AP */
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
}

/* ===== LOOP ===== */
void loop() {
  server.handleClient();

  uint16_t error;
  error = sen5x.readMeasuredValues(
            pm1, pm2_5, pm4, pm10,
            humidity, temperature,
            vocIndex, noxIndex);

  if (error) {
    Serial.println("SEN54 read error");
    return;
  }

  /* ===== ALARM LOGIC ===== */
  alarm = false;

  if (pm1 > PM_LIMIT || pm2_5 > PM_LIMIT || pm4 > PM_LIMIT || pm10 > PM_LIMIT) {
    alarm = true;
  }

  alarmStatus = alarm;   // ✅ update AFTER calculation

  /* ===== RELAY + BUZZER ===== */
  if (alarm) {
    digitalWrite(RELAY_PIN, LOW);                       // TRIP relay
    digitalWrite(BUZZER_PIN, millis() % 1000 < 500);    // Buzzer beep
  } else {
    digitalWrite(RELAY_PIN, HIGH);                      // NORMAL
    digitalWrite(BUZZER_PIN, LOW);                      // Buzzer OFF
  }

  /* ===== TFT UPDATE ===== */
  tft.fillRect(0, 40, 320, 200, TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  int y = 45;
  tft.setCursor(10, y); tft.printf("PM1.0 : %.1f ug/m3", pm1/100);    y += 25;
  tft.setCursor(10, y); tft.printf("PM2.5 : %.1f ug/m3", pm2_5/100);  y += 25;
  tft.setCursor(10, y); tft.printf("PM4.0 : %.1f ug/m3", pm4/100);    y += 25;
  tft.setCursor(10, y); tft.printf("PM10  : %.1f ug/m3", pm10/100);   y += 30;

  tft.setCursor(10, y); tft.printf("Temp  : %.1f C", temperature); y += 25;
  tft.setCursor(10, y); tft.printf("Hum   : %.1f %%", humidity);   y += 30;

  tft.setTextColor(alarm ? TFT_RED : TFT_GREEN, TFT_BLACK);
  tft.setCursor(10, y);
  tft.printf("VOC Index : %.0f", vocIndex);

  delay(3000);
}
