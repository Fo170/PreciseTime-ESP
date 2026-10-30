# PreciseTime-ESP - Librairie de chronométrage haute précision pour ESP8266/ESP32

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Arduino](https://img.shields.io/badge/Arduino-IDE-00979C?style=flat&logo=arduino)](https://docs.arduino.cc/software/ide/)
[![Arduino Badge](https://img.shields.io/badge/framework-arduino-brightgreen?logo=arduino.svg)](https://www.arduino.cc/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-compatible-brightgreen.svg)](https://platformio.org/)
[![ESP8266](https://img.shields.io/badge/ESP8266-Supported-green.svg)](https://www.espressif.com/)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![Arduino Library Badge](https://www.ardu-badge.com/badge/PreciseTime-ESP.svg)](https://github.com/Fo170?tab=repositories)
[![Version](https://img.shields.io/badge/version-1.0.4-blue.svg)](https://github.com/Fo170/PreciseTime-ESP/releases)
[![GitHub release](https://img.shields.io/github/v/release/Fo170/PreciseTime-ESP)](https://github.com/Fo170/PreciseTime-ESP/releases)
![GitHub last commit](https://img.shields.io/github/last-commit/Fo170/PreciseTime-ESP)


Une librairie Arduino/PlatformIO pour un chronométrage ultra-précis avec gestion automatique des débordements, idéale pour les applications nécessitant une mesure du temps fiable sur de longues périodes.

**Version actuelle : 1.0.4 (Octobre 2026)**

## ✨ Fonctionnalités

- ⏱️ **Haute précision** : Résolution à la microseconde sur ESP32
- 🔄 **Gestion des débordements** : Support jusqu'à 584,942 années
- 🎯 **Multi-plateforme** : Support ESP8266, ESP32 et Arduino
- 🔒 **Thread-safe** : Sections critiques pour ESP32
- 📊 **Interface riche** : Formatage en chaîne, composantes individuelles
- ⚡ **Faible overhead** : Interruptions optimisées
- 🩺 **Auto-échantillonnage** : base de temps entretenue sans appelant, indépendante du WiFi et du NTP
- 🚨 **Détection des trous** : `getLostSamples()` / `isReliable()` signalent une mesure douteuse

## 📦 Installation

### Via PlatformIO (recommandé)

Ajoutez la dépendance à votre `platformio.ini` :

```ini
lib_deps = 
    https://github.com/Fo170/PreciseTime-ESP.git@^1.0.4
```

## ⚠️ Échantillonnage et compteur 32 bits (ESP8266)

`micros()` sur ESP8266 est un compteur **32 bits** qui repart à zéro toutes les
**4 294 967 296 µs = 71,58 min** (1 h 11 min 35 s). La librairie accumule par
delta non signé, ce qui rend le passage à zéro transparent — mais seulement si
deux échantillonnages consécutifs sont espacés de **moins de la moitié** de
cette période (règle de Nyquist, **35,79 min**).

Au-delà, le nombre de tours est perdu : l'erreur vaut un multiple exact de
71,58 min. Un trou de lecture de 3 h produit ainsi une erreur de **143,17 min**.

La librairie s'échantillonne donc elle-même : `maintien()` committe
l'accumulateur au plus tard toutes les `PRECISETIME_SAMPLE_MS` (60 s par
défaut), et le compteur est **armé dès `begin()`**. Il suffit d'appeler
`PreciseTime::update()` dans `loop()` — aucun réseau n'est requis, une coupure
WiFi de plusieurs jours est sans effet.

```ini
build_flags = -DPRECISETIME_SAMPLE_MS=5000UL
```

La valeur doit rester très inférieure à 35 791 ms.

```cpp
void loop() {
    PreciseTime::update();
    if (!PreciseTime::isReliable()) {
        // getLostSamples() > 0 : une mesure est douteuse, à signaler
    }
}
```

## SYNTHÈSE FINALE & RECOMMANDATIONS ##

# Verdict Comparatif Final

| Critère | PreciseTime-ESP v1.0.4 |
|:--------|:----------------------:|
| Précision ESP32 | ⭐⭐⭐⭐⭐ 1 µs (timer hardware 80 MHz / 80) |
| Précision ESP8266 | ⭐⭐⭐⭐⭐ ~4 µs (accumulation par delta de micros()) |
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
| Maintenance active | ✅ v1.0.4 (octobre 2026) |
| Documentation | Complète |

| Plateforme  |  Mémoire RAM statique |
| ----------- |  ---------------------- |
| **ESP32**   |  ~24 octets  |
| **ESP8266** |  ~20 octets  |
| **Arduino** |  ~12 octets  |


| Plateforme  |  CPU (cycles par appel)   |
| ----------- |  ------------------------ |
| **ESP32**   |  ~30-50 cycles  |
| **ESP8266** |  ~15-30 cycles  |
| **Arduino** |  ~10-20 cycles  |

Analyse technique :
ESP32 : Timer hardware dédié (timerBegin(0, 80, true)), ISR incrémente un compteur 64 bits à chaque µs. Précision exacte.

ESP8266 : micros() natif (32 bits) + accumulation par delta non signé (aucun timer, aucune interruption). Le passage à zéro du compteur, toutes les 71,58 min, est transparent tant que deux échantillonnages restent espacés de moins de 35,79 min — d'où l'auto-échantillonnage via `PRECISETIME_SAMPLE_MS`. La résolution native de micros() sur ESP8266 est ~4 µs, pas 1 µs.

Arduino : Fallback sur millis() → précision divisée par 1000.

## 🏆 Recommandation

Meilleure optimisation

Projet multi-plateforme (AVR/STM32/SAM/ESP8266/ESP32)

Performance critique sur ESP8266/ESP32
(inlining réduit l'overhead de ~30-40%)

## Dépôt GitHub

Poussé sur : https://github.com/Fo170/PreciseTime-ESP
