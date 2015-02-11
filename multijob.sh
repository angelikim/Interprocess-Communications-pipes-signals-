
#!/bin/bash 

for i
do
 while read LINE  
 do
  $LINE;
 done<$1
done
exit



