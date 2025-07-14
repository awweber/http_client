#ifndef CONFIG_H
#define CONFIG_H

// ========================================
// HTTP-EINSTELLUNGEN
// ========================================

// Standard-URL für Tests (httpbin.org ist ein kostenloser HTTP-Test-Service)
#define HTTP_URL "http://httpbin.org/post"

// HTTP-Timeout in Sekunden
#define HTTP_TIMEOUT 10

// ========================================
// SENSOR-EINSTELLUNGEN
// ========================================

// Intervall zwischen Messungen (in Sekunden)
#define SENSOR_READ_INTERVAL 5

// ========================================
// DEVICE-EINSTELLUNGEN
// ========================================

// Eindeutige Geräte-ID (ändere diese für jedes Gerät)
#define DEVICE_ID "raspi_sensor_01"

// ========================================
// SIMULATION-EINSTELLUNGEN
// ========================================

// Temperaturbereich für Simulation (Min - Max °C)
#define TEMP_MIN 20.0f
#define TEMP_MAX 30.0f

// Luftfeuchtigkeitsbereich für Simulation (Min - Max %)
#define HUMIDITY_MIN 40.0f
#define HUMIDITY_MAX 70.0f

// Druckbereich für Simulation (Min - Max hPa)
#define PRESSURE_MIN 1000.0f
#define PRESSURE_MAX 1100.0f

#endif /* CONFIG_H */
