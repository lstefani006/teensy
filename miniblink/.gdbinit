target extended-remote /dev/ttyBmpGdb
mon swdp_scan
attach 1
set mem inaccessible-by-default off
monitor vector_catch disable hard 
monitor option erase
set print pretty
load

