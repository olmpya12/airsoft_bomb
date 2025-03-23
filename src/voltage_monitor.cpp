#include "voltage_monitor.h"
#include "config.h"
#include "settings.h"
#include <Arduino.h>

VoltageMonitor::VoltageMonitor() : lastReading(0.0), referenceVoltage(5.0), resistorDivider(2), sensorPin(A0) {}

void VoltageMonitor::init() {
    pinMode(sensorPin, INPUT);
    // Set ADC reference to external reference (if applicable)
    // analogReference(EXTERNAL);
    
    // Take initial reading
    lastReading = readVoltage();
}

float VoltageMonitor::readVoltage() {
    // Read analog value
    int analogValue = analogRead(sensorPin);
    
    // Convert to voltage (Arduino UNO has 10-bit ADC, so max value is 1023)
    // Multiply by resistor divider if applicable
    float voltage = (analogValue / 1023.0) * referenceVoltage * resistorDivider;
    
    // Update last reading
    lastReading = voltage;
    
    return voltage;
}

bool VoltageMonitor::isLow() {
    return false; // Implement this based on your battery voltage threshold
}