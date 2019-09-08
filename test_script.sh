#!/bin/bash

# ./test_script.sh 1 | tee DB32.txt;  ./test_script.sh 2 | tee DB64.txt; ./test_script.sh 3 | tee RE32.txt; ./test_script.sh 4 | tee RE64.txt

# diff DB64.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
# diff RE32.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
# diff DB32.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
# diff DB32.txt DB64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
# diff RE32.txt DB64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
# diff RE32.txt DB32.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l

DB32="./Debug/hw1-1.exe"
DB64="./x64/Debug/hw1-1.exe"
RE32="./Release/hw1-1.exe"
RE64="./x64/Release/hw1-1.exe"

PROGRAM=$DB32

if   [[ $1 -eq 1 ]]; then
	PROGRAM=$DB32
elif [[ $1 -eq 2 ]]; then
	PROGRAM=$DB64
elif [[ $1 -eq 3 ]]; then
	PROGRAM=$RE32
elif [[ $1 -eq 4 ]]; then
	PROGRAM=$RE64
fi

echo "$PROGRAM"

for i in "http:tamu.edu:0" "http://tamu.edu:0" "http://tamu.edu:-5/" "http://www.washington.edu/" "http://www.washington.edu:443" "http://www.nyu.edu/" "http://tamu.edu" "http://www.tamu.edu:80" "http://128.194.135.72:80/courses/index.asp#location" "http://165.91.22.70/" "http://s6.irl.cs.tamu.edu/IRL140M.htm" "http://s6.irl.cs.tamu.edu/IRL96M.htm" "http://csnet.cs.tamu.edu:443?addrbook.php" "http://smtp-relay.tamu.edu:465/index.html" "http://ftp.gnu.org:21/" "http://s22.irl.cs.tamu.edu:990/view?test=1" "http://128.194.135.11?viewcart.php/"; do
	printf "\n\n%s\n" "------------------------"
	echo "$i"
	echo "------------------------"
	$PROGRAM $i
	
done;

