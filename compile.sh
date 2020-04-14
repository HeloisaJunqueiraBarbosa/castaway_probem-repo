#! /bin/bash 

cd world
qmake-qt5
make -j8
cd ..

cd viewer
qmake-qt5
make -j8
cd ..

cd shark
qmake-qt5
make -j8
cd ..

cd human
qmake-qt5
make -j8
cd ..
