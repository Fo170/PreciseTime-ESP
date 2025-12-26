# PreciseTime - Librairie de chronométrage haute précision pour ESP8266/ESP32

[![PlatformIO](https://img.shields.io/badge/PlatformIO-compatible-brightgreen.svg)](https://platformio.org/)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/)
[![ESP8266](https://img.shields.io/badge/ESP8266-Supported-green.svg)](https://www.espressif.com/)
![GitHub release](https://img.shields.io/github/v/release/Fo170/PreciseTime-ESP)
![GitHub last commit](https://img.shields.io/github/last-commit/Fo170/PreciseTime-ESP)
![PlatformIO Registry](https://badges.registry.platformio.org/packages/Fo170/library/PreciseTime.svg)
![GitHub stars](https://img.shields.io/github/stars/Fo170/PreciseTime-ESP?style=social)

Une librairie Arduino/PlatformIO pour un chronométrage ultra-précis avec gestion automatique des débordements, idéale pour les applications nécessitant une mesure du temps fiable sur de longues périodes.

**Version actuelle : 1.0.1 (Décembre 2025)**

## ✨ Fonctionnalités

- ⏱️ **Haute précision** : Résolution à la microseconde sur ESP32
- 🔄 **Gestion des débordements** : Support jusqu'à 584,942 années
- 🎯 **Multi-plateforme** : Support ESP32, ESP8266 et Arduino
- 🔒 **Thread-safe** : Sections critiques pour ESP32
- 📊 **Interface riche** : Formatage en chaîne, composantes individuelles
- ⚡ **Faible overhead** : Interruptions optimisées

## 📦 Installation

### Via PlatformIO (recommandé)

Ajoutez la dépendance à votre `platformio.ini` :

```ini
lib_deps = 
    https://github.com/Fo170/PreciseTime-ESP.git@^1.0.1
