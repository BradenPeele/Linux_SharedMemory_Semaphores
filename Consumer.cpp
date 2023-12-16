#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<unistd.h>
#include<iostream>
#include<semaphore.h>
#include<bits/stdc++.h>
#include<fstream>
#include<bits/stdc++.h>

using namespace std;

string decrypt(string);
void storeInFile(vector<string>);

int main(){
	
	const int SIZE = 4096;
	const char * name = "OS";
	
	int shm_fd;
	char * ptr;
	char * loc;  //we are going to use this as the current location in shared memory
	
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, SIZE);
	ptr = (char *)mmap(0,SIZE,PROT_WRITE, MAP_SHARED, shm_fd,0);
	loc = ptr;
	
	sem_t *sem1 = sem_open("mutex1", 0); /* Open a preexisting semaphore. */
	sem_t *sem2 = sem_open("mutex2", 0); /* Open a preexisting semaphore. */
	
	vector<string> v;
	while(true) {
		//wait for producer to notify
   		sem_wait(sem1);
   		if(strstr(loc, "==done=="))
   			break;
   		string str = loc;
   		string s;
		stringstream ss(str);
		while(getline(ss, s, ' ')) {
			v.push_back(decrypt(s));
		}
		
		sem_post(sem2);
	}
	
	storeInFile(v);
	
	//frees up the shared memory, the OS will free up the memory when there are no more links to it
	shm_unlink(name);
	
	return 0;
}


void storeInFile(vector<string> v) {
	const char * filename = "output";
	// open file
	ofstream outFile(filename);
	if(!outFile.is_open()) {
		cout << "File error" << endl;
		exit(EXIT_FAILURE);
	}
	
	for(int n = 0; n < v.size(); n++) {
		outFile << v[n] << endl;
	}
	
	outFile.close();
}


string decrypt(string password) {
	int length = password.length();
	char* charArr = new char[length+1];
	strcpy(charArr, password.c_str());
	
	// last char stored
	char temp = charArr[0];

	// shift letters
	for(int n = 0; n < length-1; n++)
		charArr[n] = charArr[n+1];
		
	// reassign temp
	charArr[length-1] = temp;
	
	string decPassword = "";
	// increment letter values
	for(int n = 0; n < length; n++) {
		charArr[n] = charArr[n] - 1;
		decPassword += charArr[n];
	}
	
	return decPassword;
}
