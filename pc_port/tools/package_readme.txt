Xenogears PC port -- test build @BUILD_ID@
commit @COMMIT@

This is an early test build of a native Linux port of Xenogears (USA,
SLUS-00664) built from the decompilation. It contains no game data: you
supply files from your own copy of the game.

WHAT YOU NEED (put them all in one folder)
  File                      Size (bytes)  SHA-256 (or SHA-1 for the image)
  disc1.bin                  718,738,272  sha1 12db8ccb93516c391630f046a143762337cc21f4
                                          (Disc 1 as a single raw MODE2/2352 BIN track,
                                           Redump redump.org/disc/177; from a CHD:
                                           chdman extractcd -i "Xenogears (USA) (Disc 1).chd"
                                           -o disc1.cue -ob disc1.bin)
  SLUS_006.64                    303,104  dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119
  field.bin                      260,862  38a1ce829a6f094c505f67143d6ace2d328418c65425a7383991179467e1fdfc
  menu.bin                       153,864  82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d
  shop_menu.bin                   55,296  7890e14bcabddcf85368de10783ecff8daa166dc5ac254e06db5e27959cab4cf
  member_change_menu.bin          26,624  3b9e2b890c27ae0de97fe343ac75cd35c05fe7f9605ae387d2166da0be78109c
  world_map.bin                  180,422  4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70
  scph5500.bin                   524,288  11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef
                                          (PS1 BIOS, for the kanji font ROM)
  SLUS_006.64 and the *.bin overlays are files inside the disc image; the
  decomp repository's tools/scripts/extract_exe.py and extract_overlays.py
  write them from disc1.bin. The port checks every hash on start and stops
  with a message naming any file that is missing or different.

RUNNING
  ./xenogears.sh /path/to/folder      first run: remembers the folder
  ./xenogears.sh                      later runs
  XENOGEARS_WINDOWED=1 ./xenogears.sh windowed instead of fullscreen
  Settings: copy config.example.ini to ~/.config/xenogears-port/config.ini.
  Saves, memory cards (memcards/card1.mcd) and the F7/F8 quick checkpoint
  are kept in ~/.local/share/xenogears-port/.
  Gamescope / Steam: add xenogears.sh as a non-Steam game, or point a
  launcher (Banshee) at it with the folder as its argument.

HOW FAR IT PLAYS (tested on the build machine)
  Boot, title screen, New Game, the opening, into Lahan (field map 2).
  The world map runs partly on hand-written port code: walking from the
  world map onto the Lahan entrance works in tests; other world-map
  transitions (and the trip to Black Moon Forest) are not tested yet.
  Battles run the retail battle code in a MIPS interpreter.

KNOWN ISSUES
  - Some world-map states and sound-bank switches are still incomplete or
    silent (docs/port/RETAIL_DIVERGENCES.md in the repository).
  - Widescreen, 60 fps and volume settings are not implemented.
  - Movie playback has not been tested.
  - A few menus run port code that is not yet verified against retail.

REPORTING BUGS
  Say what you did and what happened, and include version.txt, the last
  lines of the terminal output (run xenogears.sh from a terminal to see it),
  and, if you can, the quick checkpoint (F7 saves it) from
  ~/.local/share/xenogears-port/quicksaves/quick.xgqs.
