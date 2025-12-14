# Changelog

Tous les changements notables de ce projet seront documentés dans ce fichier.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/),
et ce projet suit le [Semantic Versioning](https://semver.org/lang/fr/).

## [1.0.0] - 2025-01-01

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