sudo apt-get install autoreconf
sudo apt-get install qemu binutils-mingw-w64 gcc-mingw-w64 xorriso mtools
cd mkgpt
autoreconf --install
./configure && make && sudo make install
cd ..
sudo apt-get install gnu-efi
export DISPLAY=:0
