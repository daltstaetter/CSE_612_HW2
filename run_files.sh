#!/bin/bash

TMP_EXE=./local_exe
PROGRAM=$TMP_EXE/hw1-32db.exe
THREADS=1

if   [[ $1 -eq 1 ]]; then
	PROGRAM=$TMP_EXE/hw1-32db.exe
elif [[ $1 -eq 2 ]]; then
	PROGRAM=$TMP_EXE/hw1-32db.exe
elif [[ $1 -eq 3 ]]; then
	PROGRAM=$TMP_EXE/hw1-32rel.exe
elif [[ $1 -eq 4 ]]; then
	PROGRAM=$TMP_EXE/hw1-64rel.exe
fi

if   [[ $2 -eq 100 ]]; then
	FILE=URL-input-100-me.txt
else
	FILE=URL-input-1M.txt
fi

echo "$PROGRAM $FILE"
$PROGRAM $THREADS $FILE



