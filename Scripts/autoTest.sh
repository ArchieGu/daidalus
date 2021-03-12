#!/bin/bash 

input="./filename.txt" 
while IFS= read -r line
do
   echo "$line"
   ./daa-algorithm/C++/DaidalusExample --testcase "$line"

done < "$input"

logDir="./logfiles"
mkdir -p ScenarioFiles 
mkdir -p ConfigFiles
for file in $logDir/*
do
    echo "$file"
    perl ./daa-algorithm/Scripts/daidalize.pl $file
    mv *.daa ./ScenarioFiles
    mv *.conf ./ConfigFiles
done 

cp ./ScenarioFiles/* ./autoTestFiles

testDir="./autoTestFiles"
for file in $testDir/*
do
    prefix=(${file//./ }) 
    ./daa-algorithm/C++/DaidalusAlerting --conf ./ConfigFiles/DO_365B_no_SUM.conf "."${prefix[0]}".daa"
done

mkdir -p TestResults
mv *.csv ./TestResults

