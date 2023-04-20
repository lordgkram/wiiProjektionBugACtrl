# Wii Projektion BugACtrl

Ein Wii-Kontroler für den ProjektionTV Bagger.

## Was ist das?
### Der ProjektionTV Bagger
Der ProjektionTV Bagger ist ein Klembausteinbagger den der Ilja(ProjektionTV) im Strem zusammengebaut hat.
Diesen kann man als Zuschauer im Strem kontrolieren kann.

### Was ist dieser Kontroller
Dieser kontroller erlaubt es den ProjektionTV Bagger mit einer Wii zu kontrollieren.

## Benötigt
- Eine WiiFB mit Nunchuck.
- Eine Wii mit HomeBrew
- Ein Token von dem Bagger-Portal (dieser wird einkompiliert)
- DevkitPro
    - DevkitPPC
    - wii sdl
- Clang

### für das host system
- SDL 1.2
    - SDL_net 1.2

## Einstellungen
Zum kompilieren von dem Nativen code wird `src/config.h` benörigt welche manuell erschaffen werden muss.
In dieser Datei steht wo die Porxy zu finden ist.
Eine vorgage ist `src/cnfigExample.h`.

Zum ausführen von `proxy.js` wird `apiKey.json` benötigt: vorlage `apiKeyExample.json`

## Ausführen
Zum ausführen des Kompilierten code muss man ebenfalls vorher die proxy(`proxy.js`) starten.
Diese Proxy wandelt die einfachen befehle des Clienten in befehle für die Bagger api um.

## Kompilation
```sh
# erschafung des build ordners
make build

# für das host system
make host

# für die wii
## wenn `DEVKITPRO` im env ist (standart installation)
make wii
## eigene DEVKITPRO installation
DEVKITPRO=/pfad/zu/devkitpro make wii
```
