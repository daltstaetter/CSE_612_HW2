#!/bin/bash

./test_script.sh 1 | tee DB32.txt  
./test_script.sh 2 | tee DB64.txt 
./test_script.sh 3 | tee RE32.txt 
./test_script.sh 4 | tee RE64.txt



diff DB64.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
diff RE32.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
diff DB32.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
diff DB32.txt DB64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
diff RE32.txt DB64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l
diff RE32.txt DB32.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9]*)" | wc -l

