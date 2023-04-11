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
- SDL1.2

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
