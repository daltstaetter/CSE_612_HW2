#!/bin/bash


DB32="./Debug/hw1.exe"
DB64="./x64/Debug/hw1.exe"
RE32="./Release/hw1.exe"
RE64="./x64/Release/hw1.exe"

TMP_EXE="./local_exe"
mkdir -p $TMP_EXE

# copy files to a local temp
echo "copy files to $TMP_EXE"
cp $DB32 $TMP_EXE/hw1-32db.exe
cp $DB64 $TMP_EXE/hw1-64db.exe
cp $RE32 $TMP_EXE/hw1-32rel.exe
cp $RE64 $TMP_EXE/hw1-64rel.exe
date | tee $TMP_EXE/timestamp.txt

#./test_script.sh 1 | tee DB32.txt
#./test_script.sh 2 | tee DB64.txt
#./test_script.sh 3 | tee RE32.txt
#./test_script.sh 4 | tee RE64.txt

#dos2unix DB32.txt
#dos2unix DB64.txt
#dos2unix RE32.txt
#dos2unix RE64.txt

#mv DB32.txt DB32_orig.txt
#mv DB64.txt DB64_orig.txt
#mv RE32.txt RE32_orig.txt
#mv RE64.txt RE64_orig.txt



#./output_compare.sh
