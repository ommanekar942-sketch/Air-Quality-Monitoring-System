## Overview

The **SEN54 Air Quality Monitoring and Protection System** is an IoT-based environmental monitoring solution developed using the **ESP32** and **Sensirion SEN54** sensor. The system continuously monitors particulate matter concentration, temperature, humidity, and VOC levels while providing local and wireless monitoring capabilities.

Real-time sensor data is displayed on a TFT LCD screen and simultaneously hosted on a Wi-Fi-based web dashboard. When air quality exceeds predefined safety limits, the system automatically activates an alarm and trips a relay for protective action.

---

## Features

* Real-time monitoring of air quality parameters
* PM1.0, PM2.5, PM4.0, and PM10 measurement
* Temperature and humidity monitoring
* VOC (Volatile Organic Compound) index monitoring
* TFT LCD live data display
* ESP32 Wi-Fi Access Point mode
* Mobile-friendly web dashboard
* Relay-based protection mechanism
* Audible buzzer alarm
* Standalone operation without internet connectivity

---

## Hardware Components

| Component               | Quantity    |
| ----------------------- | ----------- |
| ESP32 Development Board | 1           |
| Sensirion SEN54 Sensor  | 1           |
| TFT LCD Display         | 1           |
| Relay Module            | 1           |
| Buzzer                  | 1           |
| Power Supply Circuit    | 1           |
| Perfboard PCB           | 1           |
| Connecting Wires        | As Required |

---

## System Architecture

```text
              +------------------+
              |     SEN54        |
              | Air Quality      |
              | Sensor           |
              +--------+---------+
                       |
                     I2C
                       |
                       v

              +------------------+
              |      ESP32       |
              +--------+---------+
                       |
        +--------------+--------------+
        |                             |
        v                             v

   +----------+               +---------------+
   | TFT LCD  |               | Wi-Fi AP Mode |
   +----------+               +---------------+
                                      |
                                      v

                            Web Dashboard Access

                       |
                       v

                 Alarm Decision Logic

                       |
            +----------+----------+
            |                     |
            v                     v

         Relay                Buzzer
```

---

## Pin Configuration

### SEN54 Connections

| SEN54 Pin | ESP32 Pin |
| --------- | --------- |
| SDA       | GPIO21    |
| SCL       | GPIO22    |
| VCC       | 5V        |
| GND       | GND       |

### Relay Connections

| Relay Pin | ESP32 Pin |
| --------- | --------- |
| IN        | GPIO13    |
| VCC       | 5V        |
| GND       | GND       |

### Buzzer Connections

| Buzzer Pin | ESP32 Pin |
| ---------- | --------- |
| Signal     | GPIO12    |
| VCC        | 5V        |
| GND        | GND       |

---

## Software Requirements

* Arduino IDE
* ESP32 Board Package
* TFT_eSPI Library
* Sensirion SEN5x Library
* WiFi Library
* WebServer Library

---

## Wi-Fi Configuration

The ESP32 operates as a local Wi-Fi Access Point.

```cpp
const char* ssid = "SEN54_MONITOR";
const char* password = "12345678";
```

### Connection Steps

1. Power the system.
2. Connect a mobile phone or laptop to:

```text
SSID: SEN54_MONITOR
Password: 12345678
```

3. Open a web browser and navigate to:

```text
http://192.168.4.1
```

4. View real-time air quality data.

---

## Parameters Monitored

The system continuously measures:

* PM1.0 (µg/m³)
* PM2.5 (µg/m³)
* PM4.0 (µg/m³)
* PM10 (µg/m³)
* Temperature (°C)
* Relative Humidity (%)
* VOC Index

---

## Alarm Logic

The protection system activates whenever any particulate matter value exceeds the predefined threshold.

```cpp
#define PM_LIMIT 4000
```

### Alarm Actions

When threshold is exceeded:

* Relay is tripped
* Buzzer starts beeping
* Alarm status displayed on TFT
* Alarm status shown on web dashboard

### Normal Condition

* Relay remains ON
* Buzzer remains OFF
* Dashboard displays normal status

---

## TFT Display

The TFT LCD displays:

* PM1.0
* PM2.5
* PM4.0
* PM10
* Temperature
* Humidity
* VOC Index

The display updates continuously with live sensor measurements.

---

## Web Dashboard

The embedded web server provides:

* Real-time sensor readings
* Air quality status indication
* Relay status monitoring
* Buzzer status monitoring
* Automatic page refresh every 2 seconds

---

## Sample Output

```text
PM1.0  : 5.8 µg/m³
PM2.5  : 6.1 µg/m³
PM4.0  : 6.1 µg/m³
PM10   : 6.1 µg/m³

Temperature : 27.0 °C
Humidity    : 49.9 %

VOC Index   : 35

Status      : AIR QUALITY NORMAL
Relay       : ON
Buzzer      : OFF
```


## Future Improvements

* Cloud data logging
* MQTT integration
* Mobile application
* AQI calculation
* Historical trend graphs
* SD card data storage
* SMS and Email alerts
* Remote monitoring dashboard

---

## Applications

* Indoor Air Quality Monitoring
* Industrial Environment Monitoring
* Smart Buildings
* Laboratories
* Educational Projects
* IoT Research and Development

---
