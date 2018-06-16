# SaturnDev
Will update if this starts to become anything resembling a game

How to build:

1. Unzip [the devkit](https://drive.google.com/open?id=1fNIpJq4SX3QqZwSqC3Io8WP0JuNb7Rh5) to the root of your C drive

2. Install MinGW's "msys" package (or find some other way to get GNU Make under Windows)

3. Add "C:\MinGW\msys\1.0\bin" and "C:\saturn\SH_COFF\sh-coff\bin" to your path

4. Download the source code, put it in the "project" folder in the devkit

5. Run the "buildcd.bat" file to automatically compile the game and write it to an ISO, openable in most emulators besides Mednafen for some reason