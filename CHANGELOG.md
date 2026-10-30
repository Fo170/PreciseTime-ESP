# Changelog

Tous les changements notables de ce projet seront documentés dans ce fichier.

Le format est basé sur [Keep a Changelog](https://keepachangelog.com/fr/1.0.0/),
et ce projet suit le [Semantic Versioning](https://semver.org/lang/fr/).

## [1.0.4] - 2026-10-30

### 🐛 Corrigé — recyclage de `micros()` toutes les 71,58 min sur ESP8266

La branche ESP8266 activait `timer1` (`TIM_DIV16`) **sans jamais appeler
`timer1_write()`** : aucune interruption n'était levée, `overflow_counter`
restait à 0 et `updateTime()` **écrasait** `total_micros` au lieu de
l'accumuler. `getSeconds()` ne renvoyait donc que la valeur 32 bits de
`micros()`, recyclée toutes les **4 294 967 296 µs = 71,58 min** (1 h 11 min
35 s) : l'uptime repartait de zéro à chaque tour.

Correctif : accumulation par **delta non signé** de `micros()`, sans timer ni
interruption. `(uint32_t)(courant - precedent)` est exact modulo 2^32, y
compris au passage à zéro. Suppression de `overflow_counter`,
`microsOverflowISR()`, `timer1_isr_init/attachInterrupt/enable`.

### ✨ Added — échantillonnage autonome et détection des trous

Un compteur 32 bits doit être échantillonné à **moins de la moitié** de sa
période de recyclage (règle de Nyquist, soit 35,79 min). Au-delà, le nombre de
tours est perdu et l'erreur vaut un multiple de 71,58 min — un trou de lecture
de 3 h produit une erreur de 143,17 min.

- `PRECISETIME_SAMPLE_MS` (défaut **60000**, surchargeable par `-D`) : période
  maximale entre deux commits de l'accumulateur.
- `maintien()` : commit throttlé, appelé en tête de `getMicroseconds()` et de
  `update()`. Le compteur est **armé dès `begin()`** : aucun appelant n'est
  requis pour entretenir la base de temps, et rien ne dépend du WiFi ni du NTP.
- `getLostSamples()` / `isReliable()` : un delta supérieur à 2^31 µs
  (35,79 min) est ambigu et incrémente le compteur, ce qui permet de signaler
  une mesure douteuse au lieu d'afficher une valeur fausse en silence.

### 🔄 Modifié

- `getMicroseconds()` **ne modifie plus l'état** : il compose `total_micros`
  (committé) et le delta courant, ce qui conserve l'exactitude à la
  microseconde entre deux commits.
- `update()` devient sûr à appeler à chaque `loop()` (commit throttlé).
- RAM statique ESP8266 : `~16` → `~20` octets (`last_commit_ms` et
  `lostSamples` en plus, `overflow_counter` en moins).
- `reset()` réarme l'échantillonnage et remet `lostSamples` à zéro.
- Version `1.0.3` → `1.0.4` (`library.json`, `library.properties`,
  `@version` du header, `README.md`).

### ✅ Inchangé

- Branche **ESP32** (timer matériel 1 s + `timerRead`) : correcte, non modifiée.
- Branche **Arduino générique** (`millis()`, recyclage 49,7 j) : non modifiée.
- API publique existante : compatible, aucun changement d'appel.

---

## [1.0.3] - 2026-10-17

### ✨ Added — macro de version `__<LIB>_H_VERSION__`

Chaque header déclare désormais sa version via une macro préprocesseur,
source **unique** (un projet hôte peut l'agréger, ex. page `/version`).

### 🏷️ Divers

- Version `1.0.2` → `1.0.3` (`library.json` + `library.properties`)

---

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