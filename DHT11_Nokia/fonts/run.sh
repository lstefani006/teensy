set -e
set -x
make
rm -f q.txt q.cpp

echo crea un dump dei font linkati in ./font_text e fa un dump
./font_text font*.cpp > q.txt

echo partendo dal dump crea il file .cpp che utilizza 0b00000 per i font
./font_text q.txt > q.cpp
