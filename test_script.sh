#!/bin/bash


for i in "http://tamu.edu" "http://www.tamu.edu:80" "http://128.194.135.72:80/courses/index.asp#location" "http://165.91.22.70/" "http://s6.irl.cs.tamu.edu/IRL140M.htm" "http://s6.irl.cs.tamu.edu/IRL96M.htm" "http://csnet.cs.tamu.edu:443?addrbook.php" "http://smtp-relay.tamu.edu:465/index.html" "http://ftp.gnu.org:21/" "http://s22.irl.cs.tamu.edu:990/view?test=1" "http://128.194.135.11?viewcart.php/"; do
	echo "./x64/Debug/463-sample.exe $i"
	./x64/Debug/463-sample.exe $i
	printf "\n\n%s\n" "------------------------"
done;

