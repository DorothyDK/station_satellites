#!/bin/bash

./station_new33
       rc=$?
       if [ $rc -ne 0 ]; then echo "ERROR, file" $fname; fi
./png_1.sh
exit 0
