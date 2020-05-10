#!/bin/bash
philoCount=$1

counter=1
while [ $counter -le $philoCount ]
do
	./dinning-p $counter $philoCount &
	((counter++))
done

echo Launched $philoCount Philosophers
