for d in $(find /home/qgu4/DAA_Package/TestADSB -maxdepth 2 -type d) 
do   
	#Do something, the directory is accessible with $d:   
	echo $d
done >filename.txt


