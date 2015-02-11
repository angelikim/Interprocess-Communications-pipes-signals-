#!/bin/bash 

a=$(./commander poll queued)
if [ "$a" != "no jobs available" ] && [ "$a" != "no processes running .." ] ;
then 
 array=(${a//\n/ })
fi
for i in "${!array[@]}"
do
 array2=(${array[i]//-/ })
 rm="./commander stop ${array2[2]}"
 $rm
 echo "./commander stop ${array2[2]}"
done
a=$(./commander poll running)
if [ "$a" != "no jobs available" ] && [ "$a" != "no processes running .." ] ;
then 
 array=(${a//\n/ })
fi
for i in "${!array[@]}"
do
 array2=(${array[i]//-/ })
 rm="./commander stop ${array2[2]}"
 $rm
 echo "./commander stop ${array2[2]}"
done
