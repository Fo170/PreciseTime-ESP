/**
 * @file PreciseTime.h
 * @brief Librairie de chronométrage haute précision pour ESP8266/ESP32
 * @version 1.0.5
 * @date 2026-10-30
 *
 * @license GPL-3.0
 *
 * Copyright (C) 2025
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * ------------------------------------------------------------------------
 * v1.0.4 — correctif ESP8266 (recyclage de micros() toutes les 71,58 min)
 *
 * Défaut corrigé (présent depuis la v1.0.0) : la branche ESP8266 activait
 * timer1 (TIM_DIV16) sans jamais appeler timer1_write(). Aucune interruption
 * n'était donc levée, overflow_counter restait à 0, et updateTime() écrasait
 * total_micros au lieu de l'accumuler. getSeconds() ne renvoyait plus que la
 * valeur 32 bits de micros(), recyclée toutes les 4 294 967 296 µs, soit
 * 71,58 min (1 h 11 min 35 s).
 *
 * Correctif : accumulation par delta non signé de micros(), sans timer ni
 * interruption. (uint32_t)(courant - precedent) est exact modulo 2^32, y
 * compris au passage à zéro.
 *
 * Limite structurelle de tout compteur 32 bits : l'accumulation par delta
 * suppose que deux échantillonnages consécutifs sont espacés de MOINS de
 * 71,58 min (règle de Nyquist : moitié de la période de recyclage, soit
 * 35,79 min). Au-delà, le nombre de tours est perdu et l'erreur vaut un
 * multiple de 71,58 min.
 *
 * Parade : la librairie s'échantillonne elle-même. maintien() committe
 * l'accumulateur au plus tard toutes les PRECISETIME_SAMPLE_MS (60 s par
 * défaut), et le compteur est armé dès begin() — aucun appelant n'est requis
 * pour entretenir la base de temps. getMicroseconds() ne modifie plus l'état :
 * il compose total_micros (committé) + le delta courant, ce qui conserve
 * l'exactitude à la microseconde entre deux commits.
 *
 * Indépendance : maintien() ne dépend ni du WiFi ni du NTP. La resynchro NTP
 * se contente de réancrer son origine sur getMicroseconds() ; l'uptime n'est
 * jamais remis à zéro, il reste monotone.
 *
 * Détection : un delta supérieur à 2^31 µs (35,79 min) est ambigu et
 * incrémente lostSamples. getLostSamples() et isReliable() permettent à
 * l'appelant de signaler une mesure douteuse plutôt que d'afficher une valeur
 * fausse en silence.
 * ------------------------------------------------------------------------
 */

#ifndef PRECISE_TIME_H
#define PRECISE_TIME_H


// Version de la librairie (source UNIQUE — agrégée par liste_lib.h)
#define __PRECISETIME_H_VERSION__ "1.0.5"

// Période d'échantillonnage du compteur micros() sur ESP8266, en millisecondes.
// Surchargeable à la compilation : -D PRECISETIME_SAMPLE_MS=5000UL
// Contrainte : rester TRÈS en dessous de 35 791 ms (moitié de la période de
// recyclage de micros(), soit 2^32 µs / 2).
#ifndef PRECISETIME_SAMPLE_MS
#define PRECISETIME_SAMPLE_MS 60000UL
#endif

#if PRECISETIME_SAMPLE_MS == 0
#error "PRECISETIME_SAMPLE_MS doit etre > 0"
#endif

#include <Arduino.h>
#include <math.h>

#if defined(ESP32)
#include "driver/timer.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#endif

class PreciseTime {
private:
    static bool initialized;

#if defined(ESP32)
    static hw_timer_t* timer;
    static volatile uint64_t time_fct_micros;
    static portMUX_TYPE timerMux;

    static void IRAM_ATTR timerISR();

#elif defined(ESP8266)
    static uint32_t last_micros;
    static uint32_t last_commit_ms;
    static uint64_t total_micros;
    static uint32_t lostSamples;

    static void updateTime();
    static void maintien();

#else
    static uint32_t last_millis;
    static uint64_t total_millis;

    static void updateSoftwareTimer();
#endif

public:
    static void begin() {
        if (initialized) return;
#if defined(ESP32)
        timer = timerBegin(0, 80, true);
        timerAttachInterrupt(timer, &timerISR, true);
        timerAlarmWrite(timer, 1000000, true);
        timerAlarmEnable(timer);
#elif defined(ESP8266)
        last_micros = micros();
        last_commit_ms = millis();
        total_micros = 0;
        lostSamples = 0;
#else
        last_millis = millis();
        total_millis = 0;
#endif
        initialized = true;
    }

    static uint64_t getMicroseconds() {
#if defined(ESP32)
        if (!initialized) return 0;
        uint64_t time;
        portENTER_CRITICAL(&timerMux);
        time = time_fct_micros + timerRead(timer);
        portEXIT_CRITICAL(&timerMux);
        return time;
#elif defined(ESP8266)
        if (!initialized) return 0;
        maintien();
        return total_micros + (uint32_t)(micros() - last_micros);
#else
        if (!initialized) return 0;
        updateSoftwareTimer();
        return total_millis * 1000ULL;
#endif
    }

    static uint64_t getMilliseconds() {
        return getMicroseconds() / 1000ULL;
    }

    static uint64_t getSeconds() {
        return getMicroseconds() / 1000000ULL;
    }

    static double getSecondsPrecise() {
        return (double)getMicroseconds() / 1000000.0;
    }

    static uint32_t getLostSamples() {
#if defined(ESP8266)
        return lostSamples;
#else
        return 0;
#endif
    }

    static bool isReliable() {
        return getLostSamples() == 0;
    }

    static void getFormattedTime(uint64_t &days, uint32_t &hours,
                                 uint32_t &minutes, uint32_t &seconds) {
        uint64_t total_seconds = getSeconds();
        days = total_seconds / 86400ULL;
        uint64_t remaining = total_seconds % 86400ULL;
        hours = remaining / 3600ULL;
        remaining %= 3600ULL;
        minutes = remaining / 60ULL;
        seconds = remaining % 60ULL;
    }

    static String getFormattedString() {
        uint64_t days;
        uint32_t hours, minutes, seconds;
        getFormattedTime(days, hours, minutes, seconds);
        char buffer[64];
        if (days > 0) {
            snprintf(buffer, sizeof(buffer), "%llu jours, %02u:%02u:%02u",
                     (unsigned long long)days, (unsigned)hours, (unsigned)minutes, (unsigned)seconds);
        } else {
            snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u",
                     (unsigned)hours, (unsigned)minutes, (unsigned)seconds);
        }
        return String(buffer);
    }

    static double getOverflowYears() {
        return (pow(2, 64) / 1000000.0 / 3600.0 / 24.0 / 365.0);
    }

    // Commit de l'accumulateur, au plus une fois toutes les
    // PRECISETIME_SAMPLE_MS. À appeler depuis loop() : rend la base de temps
    // indépendante de toute activité réseau. Sans danger en appel répété.
    static void update() {
#if defined(ESP8266)
        maintien();
#elif !defined(ESP32)
        updateSoftwareTimer();
#endif
    }

    static bool isInitialized() {
        return initialized;
    }

    static void reset() {
#if defined(ESP32)
        portENTER_CRITICAL(&timerMux);
        time_fct_micros = 0;
        timerWrite(timer, 0);
        portEXIT_CRITICAL(&timerMux);
#elif defined(ESP8266)
        last_micros = micros();
        last_commit_ms = millis();
        total_micros = 0;
        lostSamples = 0;
#else
        last_millis = millis();
        total_millis = 0;
#endif
    }
};

// Static variable definitions
bool PreciseTime::initialized = false;

#if defined(ESP32)
hw_timer_t* PreciseTime::timer = nullptr;
volatile uint64_t PreciseTime::time_fct_micros = 0;
portMUX_TYPE PreciseTime::timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR PreciseTime::timerISR() {
    portENTER_CRITICAL_ISR(&timerMux);
    time_fct_micros += 1000000;
    portEXIT_CRITICAL_ISR(&timerMux);
}

#elif defined(ESP8266)
uint32_t PreciseTime::last_micros = 0;
uint32_t PreciseTime::last_commit_ms = 0;
uint64_t PreciseTime::total_micros = 0;
uint32_t PreciseTime::lostSamples = 0;

void PreciseTime::updateTime() {
    if (!initialized) return;
    uint32_t current_micros = micros();
    uint32_t delta = (uint32_t)(current_micros - last_micros);
    if (delta > 0x80000000UL) lostSamples++;
    total_micros += delta;
    last_micros = current_micros;
    last_commit_ms = millis();
}

void PreciseTime::maintien() {
    if (!initialized) return;
    if ((uint32_t)(millis() - last_commit_ms) < PRECISETIME_SAMPLE_MS) return;
    updateTime();
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

#endif // PRECISE_TIME_H
