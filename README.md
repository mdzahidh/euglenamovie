1) Make sure the following are installed
   autoconf
   libtool
   nasm

2) First compile libjpeg-turbo
From inside libjpeg-turbo
./configure --prefix=/usr/local --disable-shared
make 
sudo make install

3) Next build the euglenamovie
mkdir build
cd build
cmake ../
make 

