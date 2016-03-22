1) First compile libjpeg-turbo
From inside libjpeg-turbo
./configure --prefix=/usr/local --disable-shared
make 
sudo make install

2) Next build the euglenamovie
mkdir build
cd build
cmake ../
make 

