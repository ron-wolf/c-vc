#include <unistd.h>		//for read()
#include <sys/types.h>	//for open()
#include <sys/stat.h>	//for open()
#include <fcntl.h>		//for open()

#include "macros.h"		//for macros

//function takes the file descriptor as an argument, returns the hash number
long long hash_fd(char * file_path){
	unsigned long hash = 0;	//unsigned long long to store the hash
	char buf[BUFF_SIZE];			//creates a buffer of the size of the macro BUFF_SIZE
	int readResult;					//int to make sure the result of read is not an error
	int fileDesc = 0;
	
	//opens the file, checks to see if it returned an error
	if((fileDesc = open(file_path, O_RDONLY)) < 0){
		LOG("Couldn't open the file to hash.");
		
		return -1;
	}
	
	//reads the file descriptor and makes sure that it is not an error
	while((readResult = read(fileDesc, buf, BUFF_SIZE)) > 0){
		//loop to multiply the current hash value by 10 and then
		//adds the ASCII value of each individual byte to the hash
		int i; for(i=0; i < readResult; i++){
			hash *= 10;
			hash += buf[i];
		}
	}
	
	//catch error if read returns -1
	if(readResult == -1){
		LOG("Couldn't read the file to hash.");
		
		return -1;
	}
	
	return (long long)hash;
}
