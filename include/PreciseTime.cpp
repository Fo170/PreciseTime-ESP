#include "PreciseTime.h"
#include <math.h>

// Static variable definitions
bool PreciseTime::initialized = false;

#if defined(ESP32)
hw_timer_t* PreciseTime::timer = nullptr;
volatile uint64_t PreciseTime::time_fct_micros = 0;
portMUX_TYPE PreciseTime::timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR PreciseTime::timerISR() {
    portENTER_CRITICAL_ISR(&timerMux);
    time_fct_micros++;
    portEXIT_CRITICAL_ISR(&timerMux);
}

#elif defined(ESP8266)
volatile uint32_t PreciseTime::overflow_counter = 0;
uint32_t PreciseTime::last_micros = 0;
uint64_t PreciseTime::total_micros = 0;

void IRAM_ATTR PreciseTime::microsOverflowISR() {
    overflow_counter++;
}

void PreciseTime::updateTime() {
    if (!initialized) return;
    uint32_t current_micros = micros();
    uint32_t high_part = overflow_counter;
    if (current_micros < last_micros) {
        high_part++;
    }
    total_micros = ((uint64_t)high_part << 32) | current_micros;
    last_micros = current_micros;
}

#else
uint32_t PreciseTime::last_millis = 0;
uint64_t PreciseTime::total_millis = 0;

void PreciseTime::updateSoftwareTimer() {
    if (!initialized) return;
    uint32_t current = millis();
    uint32_t delta;
    if (current >= last_millis) {
        delta = current - last_millis;
    } else {
        delta = (UINT32_MAX - last_millis) + current + 1;
    }
    total_millis += delta;
    last_millis = current;
}

#endif

// Remaining methods are inline in the header (or provided by original library).
