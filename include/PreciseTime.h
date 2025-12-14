/**
 * @file PreciseTime.h
 * @brief Librairie de chronométrage haute précision pour ESP8266/ESP32
 * @version 1.0.0
 * @date 2025-12-14
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
 */

#ifndef PRECISE_TIME_H
#define PRECISE_TIME_H

#include <Arduino.h>

#if defined(ESP32)
#include "driver/timer.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#endif

/**
 * @class PreciseTime
 * @brief Classe fournissant un chronométrage haute précision avec gestion des débordements
 * 
 * Cette librairie permet de mesurer le temps écoulé depuis le démarrage avec une précision
 * allant jusqu'à la microseconde sur ESP32. Elle gère automatiquement les débordements
 * des compteurs internes et fournit une interface simple pour récupérer le temps
 * dans différentes unités.
 * 
 * Caractéristiques principales:
 * - Précision microseconde sur ESP32
 * - Gestion automatique des débordements (jusqu'à 584,942 ans)
 * - Interface thread-safe
 * - Support ESP32, ESP8266 et autres architectures Arduino
 * - Formatage du temps en chaîne lisible
 */
class PreciseTime {
private:
    // Variables statiques
    static bool initialized;
    
#if defined(ESP32)
    // Variables spécifiques ESP32
    static hw_timer_t* timer;
    static volatile uint64_t time_fct_micros;
    static portMUX_TYPE timerMux;
    
    // Interruption timer ESP32
    static void IRAM_ATTR timerISR() {
        portENTER_CRITICAL_ISR(&timerMux);
        time_fct_micros++;
        portEXIT_CRITICAL_ISR(&timerMux);
    }
    
#elif defined(ESP8266)
    // Variables spécifiques ESP8266
    static volatile uint32_t overflow_counter;
    static uint32_t last_micros;
    static uint64_t total_micros;
    
    // Interruption débordement ESP8266
    static void IRAM_ATTR microsOverflowISR() {
        overflow_counter++;
    }
    
    // Mise à jour du temps ESP8266
    static void updateTime() {
        if (!initialized) return;
        
        uint32_t current_micros = micros();
        uint32_t high_part = overflow_counter;
        
        // Vérifier si un débordement s'est produit
        if (current_micros < last_micros) {
            high_part++;
        }
        
        // Calculer le temps total en microsecondes (64 bits)
        total_micros = ((uint64_t)high_part << 32) | current_micros;
        last_micros = current_micros;
    }
    
#else
    // Variables pour autres architectures
    static uint32_t last_millis;
    static uint64_t total_millis;
    
    // Mise à jour logicielle
    static void updateSoftwareTimer() {
        if (!initialized) return;
        
        uint32_t current = millis();
        uint32_t delta;
        
        if (current >= last_millis) {
            delta = current - last_millis;
        } else {
            // Gestion du débordement de millis()
            delta = (UINT32_MAX - last_millis) + current + 1;
        }
        
        total_millis += delta;
        last_millis = current;
    }
#endif

public:
    /**
     * @brief Initialise le système de chronométrage précis
     * 
     * Cette méthode doit être appelée une fois dans setup() avant toute utilisation
     * des autres méthodes. Elle configure le timer matériel sur ESP32 ou le système
     * de gestion des débordements sur ESP8266.
     * 
     * @note Sur ESP32, utilise le Timer 0 avec une résolution de 1µs
     * @note Sur ESP8266, utilise une interruption sur débordement de micros()
     */
    static void begin() {
        if (initialized) return;
        
#if defined(ESP32)
        // Configuration du timer ESP32 (1MHz = 1µs par tick)
        timer = timerBegin(0, 80, true); // Timer 0, prescaler 80 (80MHz/80=1MHz)
        timerAttachInterrupt(timer, &timerISR, true);
        timerAlarmWrite(timer, 1, true); // Déclenchement toutes les 1µs
        timerAlarmEnable(timer);
        
#elif defined(ESP8266)
        // Configuration pour ESP8266
        last_micros = micros();
        total_micros = 0;
        overflow_counter = 0;
        
        // Attacher une interruption au débordement de micros()
        timer1_isr_init();
        timer1_attachInterrupt(microsOverflowISR);
        timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
        
#else
        // Méthode logicielle pour autres architectures
        last_millis = millis();
        total_millis = 0;
#endif
        
        initialized = true;
    }
    
    /**
     * @brief Retourne le temps écoulé en microsecondes
     * 
     * @return uint64_t Temps en microsecondes depuis l'appel à begin()
     */
    static uint64_t getMicroseconds() {
#if defined(ESP32)
        if (!initialized) return 0;
        uint64_t time;
        portENTER_CRITICAL(&timerMux);
        time = time_fct_micros;
        portEXIT_CRITICAL(&timerMux);
        return time;
        
#elif defined(ESP8266)
        if (!initialized) return 0;
        updateTime();
        return total_micros;
        
#else
        if (!initialized) return 0;
        updateSoftwareTimer();
        return total_millis * 1000ULL; // Conversion ms → µs
#endif
    }
    
    /**
     * @brief Retourne le temps écoulé en millisecondes
     * 
     * @return uint64_t Temps en millisecondes depuis l'appel à begin()
     */
    static uint64_t getMilliseconds() {
        return getMicroseconds() / 1000ULL;
    }
    
    /**
     * @brief Retourne le temps écoulé en secondes
     * 
     * @return uint64_t Temps en secondes (entier) depuis l'appel à begin()
     */
    static uint64_t getSeconds() {
        return getMicroseconds() / 1000000ULL;
    }
    
    /**
     * @brief Retourne le temps écoulé en secondes avec décimales
     * 
     * @return double Temps en secondes avec précision microseconde
     */
    static double getSecondsPrecise() {
        return (double)getMicroseconds() / 1000000.0;
    }
    
    /**
     * @brief Récupère le temps formaté en composantes
     * 
     * @param[out] days Jours écoulés
     * @param[out] hours Heures écoulées (0-23)
     * @param[out] minutes Minutes écoulées (0-59)
     * @param[out] seconds Secondes écoulées (0-59)
     */
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
    
    /**
     * @brief Retourne une chaîne formatée du temps écoulé
     * 
     * Format: "X jours, HH:MM:SS" ou "HH:MM:SS" si moins d'un jour
     * 
     * @return String Temps formaté en chaîne lisible
     */
    static String getFormattedString() {
        uint64_t days;
        uint32_t hours, minutes, seconds;
        
        getFormattedTime(days, hours, minutes, seconds);
        
        char buffer[64];
        if (days > 0) {
            snprintf(buffer, sizeof(buffer), "%llu jours, %02lu:%02lu:%02lu", 
                     days, hours, minutes, seconds);
        } else {
            snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", 
                     hours, minutes, seconds);
        }
        
        return String(buffer);
    }
    
    /**
     * @brief Calcule le temps avant débordement du compteur
     * 
     * Le compteur utilise un uint64_t pour stocker les microsecondes.
     * Cette méthode calcule combien d'années il faudra avant le débordement.
     * 
     * @return double Nombre d'années avant débordement
     */
    static double getOverflowYears() {
        return (pow(2, 64) / 1000000.0 / 3600.0 / 24.0 / 365.0);
    }
    
    /**
     * @brief Mise à jour manuelle du compteur
     * 
     * À appeler périodiquement dans loop() pour les architectures
     * sans interruption matérielle (non-ESP).
     */
    static void update() {
#if defined(ESP8266)
        updateTime();
#elif !defined(ESP32)
        updateSoftwareTimer();
#endif
    }
    
    /**
     * @brief Vérifie si le système est initialisé
     * 
     * @return bool true si begin() a été appelé avec succès
     */
    static bool isInitialized() {
        return initialized;
    }
    
    /**
     * @brief Réinitialise le compteur de temps à zéro
     * 
     * Réinitialise toutes les variables internes. Après cet appel,
     * getMicroseconds() retournera 0 jusqu'à ce que le temps s'écoule à nouveau.
     */
    static void reset() {
#if defined(ESP32)
        portENTER_CRITICAL(&timerMux);
        time_fct_micros = 0;
        portEXIT_CRITICAL(&timerMux);
        
#elif defined(ESP8266)
        last_micros = micros();
        total_micros = 0;
        overflow_counter = 0;
        
#else
        last_millis = millis();
        total_millis = 0;
#endif
    }
};

// Initialisation des variables statiques
bool PreciseTime::initialized = false;

#if defined(ESP32)
hw_timer_t* PreciseTime::timer = nullptr;
volatile uint64_t PreciseTime::time_fct_micros = 0;
portMUX_TYPE PreciseTime::timerMux = portMUX_INITIALIZER_UNLOCKED;

#elif defined(ESP8266)
volatile uint32_t PreciseTime::overflow_counter = 0;
uint32_t PreciseTime::last_micros = 0;
uint64_t PreciseTime::total_micros = 0;

#else
uint32_t PreciseTime::last_millis = 0;
uint64_t PreciseTime::total_millis = 0;
#endif

#endif // PRECISE_TIME_H