#!/bin/sh

if "$1" == "clean"; then
        (cd julius ; make clean ; cd .. );
        (cd liblinear ; make clean ; cd ..);
#        (cd mecab ; make clean; cd ..  );
        (cd lua ; make clean; cd ..  );
        (cd naichichi2 ; make clean; cd .. );
else
        (cd julius ; ./configure --with-alsa ; make ; cd .. );
        (cd liblinear ; ./configure ; make ; cd .. );
#        (cd mecab ; ./configure ; make ; cd .. );
        (cd lua ; ./configure ; make linux ; cd ..  );
        (cd naichichi2 ; ./configure ; make ; cd .. );

fi
