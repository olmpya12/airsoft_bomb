#ifndef VOLTAGE_MONITOR_H
#define VOLTAGE_MONITOR_H

#include <Arduino.h>

class VoltageMonitor {
private:
    float lastReading;      // Last voltage reading
    float referenceVoltage; // Reference voltage for the ADC
    float resistorDivider;  // Resistor divider factor (if using voltage divider)
    int sensorPin;          // Analog pin for voltage sensing

public:
    VoltageMonitor();
    void init();
    float readVoltage();
    bool isLow();
};

#endif // VOLTAGE_MONITOR_H