# PreciseTime-ESP - Librairie de chronométrage haute précision pour ESP8266/ESP32

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Arduino](https://img.shields.io/badge/Arduino-IDE-00979C?style=flat&logo=arduino)](https://docs.arduino.cc/software/ide/)
[![Arduino Badge](https://img.shields.io/badge/framework-arduino-brightgreen?logo=arduino.svg)](https://www.arduino.cc/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-compatible-brightgreen.svg)](https://platformio.org/)
[![ESP8266](https://img.shields.io/badge/ESP8266-Supported-green.svg)](https://www.espressif.com/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Arduino Library Badge](https://www.ardu-badge.com/badge/PreciseTime-ESP.svg)](https://github.com/Fo170?tab=repositories)
![GitHub release](https://img.shields.io/github/v/release/Fo170/PreciseTime-ESP)
![GitHub last commit](https://img.shields.io/github/last-commit/Fo170/PreciseTime-ESP)


Une librairie Arduino/PlatformIO pour un chronométrage ultra-précis avec gestion automatique des débordements, idéale pour les applications nécessitant une mesure du temps fiable sur de longues périodes.

**Version actuelle : 1.0.1 (Décembre 2025)**

## ✨ Fonctionnalités

- ⏱️ **Haute précision** : Résolution à la microseconde sur ESP32
- 🔄 **Gestion des débordements** : Support jusqu'à 584,942 années
- 🎯 **Multi-plateforme** : Support ESP8266, ESP32 et Arduino
- 🔒 **Thread-safe** : Sections critiques pour ESP32
- 📊 **Interface riche** : Formatage en chaîne, composantes individuelles
- ⚡ **Faible overhead** : Interruptions optimisées

## 📦 Installation

### Via PlatformIO (recommandé)

Ajoutez la dépendance à votre `platformio.ini` :

```ini
lib_deps = 
    https://github.com/Fo170/PreciseTime-ESP.git@^1.0.1
```

## SYNTHÈSE FINALE & RECOMMANDATIONS ##

# Verdict Comparatif Final

| Critère | PreciseTime-ESP v1.0.1 |
|:--------|:----------------------:|
| Précision ESP32 | ⭐⭐⭐⭐⭐ 1 µs (timer hardware 80 MHz / 80) |
| Précision ESP8266 | ⭐⭐⭐⭐⭐ ~4 µs (software overflow via micros()) |
| Précision Arduino AVR/STM32/SAM | ⭐⭐ 1 ms (fallback millis()) |
| Non-bloquant | ✅ Oui |
| Gestion débordements | ✅ 584k ans |
| Thread-safe (ESP32) | ✅ Oui |
| Overhead CPU | ~15-50 cycles |
| RAM statique | ~24-32 octets |
| Flash code | ~500-800 B |
| Header-only | ✅ Oui |
| Inlining optimisé | ✅ Oui |
| Plateformes supportées | 5 plateformes |
| Exemples | 2 (Basic + Advanced) |
| Maintenance active | ✅ v1.0.1 (déc 2025) |
| Documentation | Complète |

| Plateforme  |  Mémoire RAM statique |
| ----------- |  ---------------------- |
| **ESP32**   |  ~24 octets  |
| **ESP8266** |  ~16 octets  |
| **Arduino** |  ~12 octets  |


| Plateforme  |  CPU (cycles par appel)   |
| ----------- |  ------------------------ |
| **ESP32**   |  ~30-50 cycles  |
| **ESP8266** |  ~15-30 cycles  |
| **Arduino** |  ~10-20 cycles  |

Analyse technique :
ESP32 : Timer hardware dédié (timerBegin(0, 80, true)), ISR incrémente un compteur 64 bits à chaque µs. Précision exacte.

ESP8266 : micros() natif (32 bits) + overflow_counter via timer1 (TIM_DIV16). La résolution native de micros() sur ESP8266 est ~4 µs, pas 1 µs.

Arduino : Fallback sur millis() → précision divisée par 1000.

## 🏆 Recommandation

Meilleure optimisation

Projet multi-plateforme (AVR/STM32/SAM/ESP8266/ESP32)

Performance critique sur ESP8266/ESP32
(inlining réduit l'overhead de ~30-40%)

## Dépôt GitHub

Poussé sur : https://github.com/Fo170/PreciseTime-ESP
