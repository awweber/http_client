# IoT Sensor HTTP Client für Raspberry Pi 5

Ein vollständiger HTTP-Client für IoT-Sensordaten, entwickelt für Raspberry Pi 5 mit Embedded Linux.

## 🎯 Projektziel

Entwicklung eines IoT-Geräts, das:
- Sensordaten erfasst (Temperatur, Luftfeuchtigkeit, Luftdruck)
- Daten über HTTP an einen Webserver sendet
- Kontinuierlich im Hintergrund läuft
- Robuste Fehlerbehandlung implementiert

## 📋 Voraussetzungen

### Hardware
- Raspberry Pi 5
- Micro-SD-Karte (min. 16GB)
- Netzwerkverbindung (Ethernet oder WiFi)
- Optional: DHT22-Sensor für echte Messungen

### Software
- Raspberry Pi OS (64-bit empfohlen)
- GCC Compiler
- libcurl4-openssl-dev
- Make

## 🚀 Installation und Setup

### 1. Auf dem Entwicklungsrechner (Linux)

```bash
# Repository klonen oder Dateien kopieren
cd ~/Code/iot_project/http_client

# Lokalen Test durchführen
make clean && make local && make test
```

### 2. Auf dem Raspberry Pi

```bash
# SSH-Verbindung zum Pi
ssh user@<RASPBERRY_PI_IP>

# Arbeitsverzeichnis erstellen
mkdir -p ~/iot_sensor
cd ~/iot_sensor

# Abhängigkeiten installieren
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev

# Dateien übertragen (vom Entwicklungsrechner)
# Methode 1: Automatisch
make install RPI_HOST=<RASPBERRY_PI_IP>

# Methode 2: Manuell
scp http_client.c Makefile config.h user@<RASPBERRY_PI_IP>:~/iot_sensor/

# Kompilieren
make local

# Ausführen
./http_client
```

## 🔧 Konfiguration

Die Konfiguration erfolgt in `config.h`:

```c
#define HTTP_URL "http://your-server.com/api/sensors"
#define SENSOR_READ_INTERVAL 30  // Messintervall in Sekunden
#define DEVICE_ID "your_device_id"
```

## 📊 Datenformat

Das Programm sendet JSON-Daten im folgenden Format:

```json
{
  "device_id": "device_001",
  "timestamp": 1742515200,
  "temperature": 23.45,
  "humidity": 65.20,
  "pressure": 1013.25
}
```

## 🔄 Als Service einrichten

Für kontinuierlichen Betrieb als Systemd-Service:

```bash
# Auf dem Raspberry Pi
sudo make service-install

# Service starten
sudo systemctl start iot-sensor

# Status prüfen
sudo systemctl status iot-sensor

# Logs anzeigen
journalctl -u iot-sensor -f
```

## 🧪 Test-URLs

Für Entwicklung und Tests:
- **httpbin.org**: http://httpbin.org/post
- **Postman Echo**: http://postman-echo.com/post
- **RequestBin**: Erstelle eigenen Endpoint auf requestbin.com

## 📁 Projektstruktur

```
http_client/
├── http_client.c    # Hauptprogramm
├── config.h         # Konfigurationsdatei
├── Makefile         # Build-System
├── README.md        # Diese Datei
└── http_client      # Kompilierte Binärdatei
```

## 🔍 Debugging

### Verbose-Modus aktivieren
Die cURL-Debug-Ausgabe ist standardmäßig aktiviert. Zum Deaktivieren:
```c
curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
```

### Log-Ausgabe überwachen
```bash
# Service-Logs live anzeigen
journalctl -u iot-sensor -f

# Letzte 50 Log-Einträge
journalctl -u iot-sensor -n 50
```

## 🛠️ Erweiterte Features

### Echte Sensoren integrieren

Für echte DHT22-Sensoren:
1. WiringPi oder ähnliche GPIO-Bibliothek installieren
2. Sensor an GPIO-Pin anschließen (siehe config.h)
3. `read_sensor_data()` Funktion anpassen

### SSL/HTTPS-Unterstützung

Für HTTPS-Endpoints:
```c
curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
```

## 🐛 Troubleshooting

### Häufige Probleme

1. **Compilation Error: curl/curl.h not found**
   ```bash
   sudo apt install libcurl4-openssl-dev
   ```

2. **Network connection failed**
   - Netzwerkverbindung prüfen: `ping google.com`
   - Server-URL testen: `curl -X POST http://httpbin.org/post`

3. **Permission denied beim Service**
   ```bash
   sudo chmod +x /home/user/iot_sensor/http_client
   sudo systemctl daemon-reload
   ```

## 📈 Nächste Schritte

- [ ] Echte Sensoren integrieren (DHT22, BMP280)
- [ ] Lokale Datenpufferung bei Netzwerkausfällen
- [ ] Web-Dashboard für Datenvisualisierung
- [ ] MQTT-Unterstützung
- [ ] Over-the-Air Updates

## 📄 Lizenz

Dieses Projekt ist für Bildungszwecke erstellt.

## 👨‍💻 Autor

Entwickelt für IoT-Übungsaufgaben und Embedded Linux-Projekte.
