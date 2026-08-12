# Changelog

Tous les changements notables de ce projet seront documentés dans ce fichier.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/),
et ce projet suit le [Semantic Versioning](https://semver.org/lang/fr/).

## [1.0.2] - 2026-08-12

### Corrigé
- Reboot en boucle sur ESP32 (task watchdog) : l'interruption du timer matériel tournait à 1 MHz (`timerAlarmWrite(timer, 1, true)`), asphyxiant le CPU
- Le timer déclenche désormais 1 interruption par seconde ; la précision microseconde est conservée en lisant le compteur matériel (`timerRead`)
- `reset()` remet aussi le compteur matériel à zéro

## [1.0.1] - 2025-12

### Ajouté
- Documentation README détaillée avec analyse comparative et recommandations

## [1.0.0] - 2025-12-14

### Ajouté
- Support complet ESP32 avec timer matériel (1µs résolution)
- Support ESP8266 avec gestion des débordements de micros()
- Support Arduino générique avec gestion logicielle
- Interface simple avec méthodes statiques
- Gestion automatique des débordements (584,942 ans)
- Formatage du temps en chaîne lisible
- Méthodes de réinitialisation et vérification
- Documentation complète en français
- Exemples BasicExample et AdvancedExample
- Tests unitaires
- GitHub Actions pour CI/CD
- Licence GPL-3.0

### Technique
- Utilisation de variables 64-bit pour éviter les débordements
- Sections critiques pour la thread-safety sur ESP32
- Interruptions optimisées pour performance
- Header-only library pour facilité d'utilisation
- Compatibilité PlatformIO et Arduino IDE