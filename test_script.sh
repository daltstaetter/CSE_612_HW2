#!/bin/bash

TMP_EXE=./local_exe
PROGRAM=$TMP_EXE/hw1-32db.exe

if   [[ $1 -eq 1 ]]; then
	PROGRAM=$TMP_EXE/hw1-32db.exe
elif [[ $1 -eq 2 ]]; then
	PROGRAM=$TMP_EXE/hw1-64db.exe
elif [[ $1 -eq 3 ]]; then
	PROGRAM=$TMP_EXE/hw1-32rel.exe
elif [[ $1 -eq 4 ]]; then
	PROGRAM=$TMP_EXE/hw1-64rel.exe
fi

echo "$PROGRAM"

for i in "http:tamu.edu:0" "http://tamu.edu:0" "http://tamu.edu:-5/" "http://www.washington.edu/" "http://www.washington.edu:443" "http://www.nyu.edu/" "http://tamu.edu" "http://www.tamu.edu:80" "http://128.194.135.72:80/courses/index.asp#location" "http://165.91.22.70/" "http://s6.irl.cs.tamu.edu/IRL140M.htm" "http://s6.irl.cs.tamu.edu/IRL96M.htm" "http://csnet.cs.tamu.edu:443?addrbook.php" "http://smtp-relay.tamu.edu:465/index.html" "http://ftp.gnu.org:21/" "http://s22.irl.cs.tamu.edu:990/view?test=1" "http://128.194.135.11?viewcart.php/"; do
	printf "\n\n%s\n" "------------------------"
	echo "$i"
	echo "------------------------"
	$PROGRAM $i
	
done;

