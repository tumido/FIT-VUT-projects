#!/bin/bash

# prepare the environment
make clean
make

# launch server
./ipkhttpserver &

for file in $(ls ./www)
do
    ./ipkhttpclient http://localhost:8080/"$file"
    output=$(diff *.payload www/"$file")

    echo -n "Test on file \"www/$file\": "

    if [[ -z $output ]]
    then
      echo 'PASS'
    else
      echo "FAIL, (here\'s the diff)"
      echo '-----------------------------------------'
      echo $output
    fi
    echo '-----------------------------------------'
    rm *.payload
done

# we are done, kill the server
kill $!

# try different port, chunksize, and delay

# try bad request

# try non-existing file

make clean
