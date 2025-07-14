/* Aufgabe:

In dieser Übungsaufgabe sollst du ein IoT-Gerät mit Embedded Linux entwickeln, das Sensordaten erfasst und 
diese über HTTP an einen Webserver sendet. Ziel ist es, die Grundlagen der IoT-Entwicklung mit Embedded Linux 
zu verstehen, einschließlich Hardware-Integration und Netzwerkkommunikation.

Schritte:
1. Einrichtung der Entwicklungsumgebung
2. Erstellung des HTTP-Clients
3. Integration eines Sensors und Erfassung von Sensordaten
4. Senden der Sensordaten über HTTP
5. Testen des IoT-Geräts
------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <curl/curl.h>
#include "config.h"

// Erweiterte Konfiguration
#define MAX_PAYLOAD_SIZE 256

// Globale Variablen für Sensordaten
typedef struct {
    float temperature;
    float humidity;
    float pressure;
    long timestamp;
} sensor_data_t;

// Funktion zum Lesen der Sensordaten (simuliert)
sensor_data_t read_sensor_data(void) {
    sensor_data_t data;
    
    // Simuliere Sensordaten mit realistischen Werten
    // In einer echten Implementierung würdest du hier echte Sensoren lesen
    data.temperature = TEMP_MIN + (rand() % (int)((TEMP_MAX - TEMP_MIN) * 10)) / 10.0f;
    data.humidity = HUMIDITY_MIN + (rand() % (int)((HUMIDITY_MAX - HUMIDITY_MIN) * 10)) / 10.0f;
    data.pressure = PRESSURE_MIN + (rand() % (int)(PRESSURE_MAX - PRESSURE_MIN));
    data.timestamp = time(NULL);
    
    return data;
}

// Callback-Funktion für HTTP-Response
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    printf("Server Response: %.*s\n", (int)total_size, (char*)contents);
    return total_size;
}

// Verbesserte Funktion zum Senden der Sensordaten
int send_sensor_data(const sensor_data_t *data) {
    CURL *curl;
    CURLcode res;
    char payload[MAX_PAYLOAD_SIZE];
    struct curl_slist *headers = NULL;
    
    // JSON-Payload erstellen
    snprintf(payload, sizeof(payload),
        "{"
        "\"device_id\":\"%s\","
        "\"timestamp\":%ld,"
        "\"temperature\":%.2f,"
        "\"humidity\":%.2f,"
        "\"pressure\":%.2f"
        "}",
        DEVICE_ID, data->timestamp, data->temperature, data->humidity, data->pressure);
    
    printf("📡 Sending sensor data: %s\n", payload);
    
    curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "❌ Failed to initialize cURL\n");
        return -1;
    }
    
    // HTTP-Headers setzen
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    
    // cURL-Optionen konfigurieren
    curl_easy_setopt(curl, CURLOPT_URL, HTTP_URL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(payload));
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, HTTP_TIMEOUT);  // Timeout aus config.h
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);   // Debug-Ausgabe aktivieren
    
    // HTTP-Request ausführen
    res = curl_easy_perform(curl);
    
    if(res != CURLE_OK) {
        fprintf(stderr, "❌ curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return -1;
    }
    
    // HTTP-Status-Code prüfen
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    printf("✅ HTTP Response Code: %ld\n", response_code);
    
    // Aufräumen
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    return (response_code >= 200 && response_code < 300) ? 0 : -1;
}

int main(int argc, char *argv[]) {
    printf("🚀 IoT Sensor Device Starting...\n");
    printf("📍 Target URL: %s\n", HTTP_URL);
    printf("⏱️  Reading interval: %d seconds\n\n", SENSOR_READ_INTERVAL);
    
    // Random-Seed für Sensorsimulation
    srand(time(NULL));
    
    // cURL global initialisieren
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Hauptschleife - kontinuierliches Lesen und Senden
    int measurement_count = 0;
    while(1) {
        measurement_count++;
        printf("\n📊 Measurement #%d\n", measurement_count);
        printf("================\n");
        
        // Sensordaten lesen
        sensor_data_t sensor_data = read_sensor_data();
        
        printf("🌡️  Temperature: %.2f°C\n", sensor_data.temperature);
        printf("💧 Humidity: %.2f%%\n", sensor_data.humidity);
        printf("🔘 Pressure: %.2f hPa\n", sensor_data.pressure);
        printf("⏰ Timestamp: %ld\n", sensor_data.timestamp);
        
        // Daten über HTTP senden
        int result = send_sensor_data(&sensor_data);
        
        if(result == 0) {
            printf("✅ Data sent successfully!\n");
        } else {
            printf("❌ Failed to send data!\n");
        }
        
        // Warten bis zur nächsten Messung
        printf("\n⏳ Waiting %d seconds until next measurement...\n", SENSOR_READ_INTERVAL);
        sleep(SENSOR_READ_INTERVAL);
    }
    
    // Cleanup (wird in dieser Endlosschleife nie erreicht)
    curl_global_cleanup();
    return 0;
}
