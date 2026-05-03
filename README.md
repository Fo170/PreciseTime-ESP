# PreciseTime - Librairie de chronométrage haute précision pour ESP8266/ESP32

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
