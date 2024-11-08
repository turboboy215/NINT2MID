# NINT2MID
Nintendo (Hirokazu Tanaka) (GB/GBC) to MIDI converter
This tool converts music from Game Boy and Game Boy Color games using Hirokazu "Hip" Tanaka's sound engine, which is used in a number of first-party games, to MIDI format.

It works with ROM images. To use it, you must specify the name of the ROM followed by the number of the bank containing the sound data (in hex). For games that contain 2 banks of music, you must run the program multiple times specifying where each different bank is located. However, in order to prevent files from being overwritten, the MIDI files from the previous bank must either be moved to a separate folder or renamed.

Note that for many games, there are "empty" tracks (usually the first or last track). This is normal.

Examples:
* NINT2MID "Super Mario Land (W) (V1.0) [!].gb" 4
* NINT2MID "Tetris (W) (V1.1) [!].gb" 2
* NINT2MID "Legend of Zelda, The - Link's Awakening DX (U) (V1.0) [C][!].gbc" 1C
* NINT2MID "Legend of Zelda, The - Link's Awakening DX (U) (V1.0) [C][!].gbc" 1F

This tool was primarily based on the following disassembly of Super Mario Land: https://github.com/kaspermeerts/supermarioland

As usual, there is another program, NINT2TXT included, which prints out information about the song data from each game. This is essentially a prototype of NINT2MID.

Supported games:
* Balloon Kid/Balloon Fight GB
* Chee-Chai Alien
* Dr. Mario
* F-1 Race
* Game Boy Camera
* Game Boy Gallery
* Game Boy Wars
* Game Boy Wars Turbo
* Golf
* Kaeruno Tameni
* The Legend of Zelda: Link's Awakening (DX)
* Lunar Chase
* Magnetic Soccer
* Radar Mission
* Super Mario Land
* Super Mario Land 2: 6 Golden Coins
* Tetris
* Tetris Attack
* Yakyuuman

## To do:
  * GBS file support
