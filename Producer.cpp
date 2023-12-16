#include<iostream>
#include<stdlib.h>
#include<string.h>
#include <cstring>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fstream>
#include<semaphore.h>

using namespace std;

string encrypt(string);

int main(){

	
	const int SIZE = 4096;
	const char * name = "OS";
	
	int shm_fd;
	char * ptr;  //we are going to use this for holding the shared memory location so we can treat it like a char array
	char * loc;  //we are going to use this as the current location in shared memory
	
	//get a file descriptor for our shared memory
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	//set the size of shared memory
	ftruncate(shm_fd, SIZE);
	//mmap returns a pointer to the beginning of the shared memory
	//(char *) casts that memory pointer to a character type 
	ptr = (char*)mmap(0,SIZE,PROT_WRITE, MAP_SHARED, shm_fd,0);
	loc = ptr;
	
	string filename;
	int numFileTransfer;
	
	 sem_t * sem1 = sem_open("mutex1", O_CREAT, 0666, 0);
	 sem_t * sem2 = sem_open("mutex2", O_CREAT, 0666, 0);
	 
	 //1 = set the semaphore to share between processes, 0 initial value
    	sem_init(sem1, 1, 0);	
    	sem_init(sem2, 1, 0);	
    	
    	cout << "Filename: ";
	cin >> filename;
	
	// open file
	ifstream inFile(filename);
	if(!inFile.is_open()) {
		cout << "File error" << endl;
		exit(EXIT_FAILURE);
	}
	
	cout << "Number of files to transfer at a time: ";
	cin >> numFileTransfer;
  
	while(!inFile.eof()) {
		string password;
		for(int n = 0; n < numFileTransfer; n++) {
			getline(inFile, password);
			password = encrypt(password) + " ";
			sprintf(loc, "%s", password.c_str());
			loc += password.length();
			if(inFile.eof()) // stop if done with file 
				break;
		}
		//increment semaphore so that the waiting process can awaken
   		sem_post(sem1);
		loc = ptr;
		sem_wait(sem2);
	}
	
	loc = ptr;
	sprintf(loc, "%s", "==done==");
	sem_post(sem1);
	inFile.close();
	
	return 0;
}

string encrypt(string password) {
	int length = password.length();
	char* charArr = new char[length+1];
	strcpy(charArr, password.c_str());
	
	// last char stored
	char temp = charArr[length-1];

	// shift letters
	for(int n = length-1; n >= 1; n--)
		charArr[n] = charArr[n-1];
		
	// reassign temp
	charArr[0] = temp;
	
	string encPassword = "";
	// increment letter values
	for(int n = 0; n < length; n++) {
		charArr[n] = charArr[n] + 1;
		encPassword += charArr[n];
	}
	
	return encPassword;
}
