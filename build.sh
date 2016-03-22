git submodule init
git submodule update
cd libjpeg-turbo
autoreconf -fiv
mkdir build
cd build
../configure --disable-shared --prefix=/usr/local
make
sudo make install
cd ../../
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release
make


