#!/bin/bash
2>/dev/nul
spacer=`printf '=%.s' {1..80}`
ok="\e[1;32mOK\e[0m"
fail="\e[1;31mFAIL\e[0m"
# prepare the environment
echo 'Set up the workspace'
echo $spacer
echo '$ make clean'
make -s clean
echo '$ make'
make -s

# launch server
echo $spacer
echo -ne "Launch server in the default configuration: \t"
./ipkhttpserver &
pid=$!
disown $pid
sleep 0.2
if [[ $? -eq 0 ]]
then
  echo -e $ok
else
  echo -e $fail
  exit $?
fi
echo $spacer

echo 'Run the basic tests'
echo $spacer
for file in $(ls ./www)
do
    ./ipkhttpclient http://localhost:8080/"$file"
    output=$(diff *.payload www/"$file")

    echo -n "Test on file \"www/$file\": "

    if [[ -z $output && -n $(ls *.payload 2>/dev/null) ]]
    then
      echo -e "\t\t$ok"
    else
      echo $output
      echo -e "\t\t$fail"
    fi
    rm *.payload
  done

echo $spacer
echo 'Clean up'
echo $spacer
# we are done, kill the server
echo "$ kill $!"
kill $pid
echo "$ make clean"
make -s clean

# try different port, chunksize, and delay
echo $spacer
echo 'Test different combinations of paramerers'
echo $spacer
for i in seq 1 3
do
  port=`shuf -i 8000-65000 -n 1`
  delay=`shuf -i 0-1000 -n 1`
  size=`shuf -i 1024-8192 -n 1`

  echo -n "Test server parameters {port: $port, delay: $delay, chunk_size: $size}: "
  ./ipkhttpserver -c "$size" -t "$delay" -p "$port" &
  pid=$!
  disown $pid
  sleep 0.2
  ./ipkhttpclient http://localhost:"$port"/chunked.txt
  output=$(diff *.payload www/chunked.txt)
  if [[ -z $output && -n $(ls *.payload 2>/dev/null) ]]
  then
    echo -e "\t$ok"
  else
    echo $output
    echo -e "\t$fail"
  fi
  rm *.payload
  kill $pid
done
echo $spacer
echo 'Clean up'
echo $spacer
echo "$ make clean"
make -s clean

