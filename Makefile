# Makefile für IoT HTTP-Client
# Unterstützt lokale Kompilierung (x86-64) und Raspberry Pi

# Compiler definieren
CC_LOCAL = gcc
CC_ARM64 = aarch64-linux-gnu-gcc

# Compiler-Flags
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lcurl

# Quell- und Zieldateien
SRC = http_client.c
TARGET_LOCAL = http_client
TARGET_ARM64 = http_client_rpi

# Standard-Target (lokale Kompilierung)
.PHONY: all local arm64 clean install test rpi-setup

all: local

# Lokale Kompilierung für x86-64
local: $(TARGET_LOCAL)

$(TARGET_LOCAL): $(SRC)
	$(CC_LOCAL) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✅ Lokale Kompilierung erfolgreich: $(TARGET_LOCAL)"

# Cross-Kompilierung für ARM64 (Raspberry Pi 5)
arm64: $(TARGET_ARM64)

$(TARGET_ARM64): $(SRC)
	@echo "⚠️  Cross-Kompilierung für ARM64..."
	@echo "   Hinweis: Falls Fehler auftreten, kompiliere direkt auf dem Raspberry Pi"
	$(CC_ARM64) $(CFLAGS) -o $@ $< $(LDFLAGS) || \
	(echo "❌ Cross-Kompilierung fehlgeschlagen. Verwende 'make rpi-setup' für Anweisungen." && false)

# Lokaler Test
test: local
	@echo "🧪 Teste lokale Version (drücke Ctrl+C zum Beenden)..."
	./$(TARGET_LOCAL)

# Installation auf Raspberry Pi (via SSH)
# Verwendung: make install RPI_HOST=192.168.1.100
install: $(SRC)
	@if [ -z "$(RPI_HOST)" ]; then \
		echo "❌ Bitte RPI_HOST angeben: make install RPI_HOST=192.168.1.100"; \
		exit 1; \
	fi
	@echo "📤 Übertrage Dateien zum Raspberry Pi $(RPI_HOST)..."
	scp $(SRC) Makefile pi@$(RPI_HOST):~/iot_sensor/
	@echo "🔨 Kompiliere auf Raspberry Pi..."
	ssh pi@$(RPI_HOST) "cd ~/iot_sensor && make local"
	@echo "✅ Installation abgeschlossen!"

# Aufräumen
clean:
	rm -f $(TARGET_LOCAL) $(TARGET_ARM64) *.o
	@echo "🧹 Aufgeräumt"

# Raspberry Pi Setup-Anweisungen
rpi-setup:
	@echo ""
	@echo "📋 RASPBERRY PI 5 SETUP-ANLEITUNG"
	@echo "=================================="
	@echo ""
	@echo "1️⃣  VORBEREITUNG AUF DEM RASPBERRY PI:"
	@echo "   ssh pi@<RPI_IP>"
	@echo "   mkdir -p ~/iot_sensor"
	@echo ""
	@echo "2️⃣  ABHÄNGIGKEITEN INSTALLIEREN:"
	@echo "   sudo apt update"
	@echo "   sudo apt install build-essential libcurl4-openssl-dev"
	@echo ""
	@echo "3️⃣  DATEIEN ÜBERTRAGEN:"
	@echo "   # Von deinem lokalen System:"
	@echo "   make install RPI_HOST=<RPI_IP>"
	@echo "   # Oder manuell:"
	@echo "   scp http_client.c Makefile pi@<RPI_IP>:~/iot_sensor/"
	@echo ""
	@echo "4️⃣  KOMPILIEREN UND AUSFÜHREN:"
	@echo "   ssh pi@<RPI_IP>"
	@echo "   cd ~/iot_sensor"
	@echo "   make local"
	@echo "   ./http_client"
	@echo ""
	@echo "5️⃣  ALS SYSTEMD-SERVICE EINRICHTEN (Optional):"
	@echo "   sudo make service-install"
	@echo ""

# Systemd Service erstellen (auf dem Raspberry Pi)
service-install:
	@echo "🔧 Erstelle systemd-Service..."
	@echo "[Unit]" | sudo tee /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "Description=IoT Sensor Data Collector" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "After=network.target" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "Wants=network.target" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "[Service]" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "Type=simple" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "User=pi" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "WorkingDirectory=/home/pi/iot_sensor" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "ExecStart=/home/pi/iot_sensor/http_client" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "Restart=always" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "RestartSec=10" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "StandardOutput=journal" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "StandardError=journal" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "[Install]" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@echo "WantedBy=multi-user.target" | sudo tee -a /etc/systemd/system/iot-sensor.service > /dev/null
	@sudo systemctl daemon-reload
	@sudo systemctl enable iot-sensor.service
	@echo "✅ Service installiert! Verwende:"
	@echo "   sudo systemctl start iot-sensor    # Service starten"
	@echo "   sudo systemctl status iot-sensor   # Status prüfen"
	@echo "   journalctl -u iot-sensor -f        # Logs anzeigen"

# Hilfe
help:
	@echo ""
	@echo "📖 VERFÜGBARE TARGETS:"
	@echo "====================="
	@echo "  make all               - Kompiliert für lokales System (x86-64)"
	@echo "  make local             - Kompiliert für lokales System (x86-64)"
	@echo "  make test              - Testet das lokale Programm"
	@echo "  make install RPI_HOST=IP - Installiert auf Raspberry Pi"
	@echo "  make clean             - Löscht kompilierte Dateien"
	@echo "  make rpi-setup         - Zeigt Raspberry Pi Setup-Anleitung"
	@echo "  make service-install   - Installiert systemd-Service (auf RPI)"
	@echo "  make help              - Zeigt diese Hilfe"
	@echo ""
