#!/bin/bash

printf "\nNew output\n"
diff -wB DB64.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l
diff -wB RE32.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l
diff -wB DB32.txt RE64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l
diff -wB DB32.txt DB64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l
diff -wB RE32.txt DB64.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l
diff -wB RE32.txt DB32.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l

printf "\nRegress\n"
diff -wB DB32.txt DB32_orig.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l
diff -wB DB64.txt DB64_orig.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l
diff -wB RE32.txt RE32_orig.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l
diff -wB RE64.txt RE64_orig.txt | egrep -v "(ms|GMT|Tag|Cookie|www|[0-9-]+|<)" | wc -l

