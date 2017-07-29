/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                               *
 * Author: Jonathan Pennington                                   *
 *                                                               *
 * Mixing up the data for assignment 4, adding duplicate records,*
 * more directories, and increasing the data record number       *
 *                                                               *
 * Used mostly to demo passing structs into pthreads to students *
 * in CS 3240 at Western Michigan University                     *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "apue.h"
#include <pthread.h>
#include <fcntl.h>

#define NUMBER_OF_FILES 50

typedef struct {
	FILE * file;
	int threadID;
} threadInfo;

// This is our thread method that increases the database index number
void * changeIndex(void*);

// This creates new files and adds data to them
void InAFile(FILE **,int);

int main(int argc, char * argv[]){

	FILE * files[NUMBER_OF_FILES];
	pthread_t threads[NUMBER_OF_FILES];
	threadInfo info[NUMBER_OF_FILES];
	struct stat data;
	int i=0;
	int j=0;
	char * line=malloc(4000);
	char * path=malloc(50);
	char * newpath=malloc(50);


	// Create directory new-data if it does not exist
	if(stat("new-data",&data)<0) if(mkdir("new-data",0777)<0) err_sys("Unable to create new-data directory");
	// Open files
	for(i=0;i<NUMBER_OF_FILES;i++){
		snprintf(path,49,"data/%d",i);
		if(!(files[i]=fopen(path,"a+"))) err_msg("Error opening file %s",path);
	}

	// Take records from files[3], put them in all the others too
	while(fgets(line,3000,files[3])){
		fputs(line,files[0]);
		fflush(files[0]);
		for(i=1;i<10;i++){
			if(i!=3){
				if(fgets(line,3000,files[3])){
					fputs(line,files[i]);
					fflush(files[i]);
				}
			}
		}
	}

	// Now do the same for 5
	rewind(files[5]);
	while(fgets(line,3000,files[5])){
		fputs(line,files[0]);
		fflush(files[0]);
		for(i=1;i<10;i++){
			if(i!=5){
				if(fgets(line,3000,files[5])){
					fputs(line,files[i]);
					fflush(files[i]);
				}
			}
		}
	}

	// Rewind all files
	for(i=0;i<10;i++) rewind(files[i]);

	// Take the records from each file and put them in a bunch of other files
	for(i=10;i<NUMBER_OF_FILES;i++){
		InAFile(files,i);
	}

	// Now we use threads to change the data in the files
	for(i=10;i<NUMBER_OF_FILES;i++){
		info[i].file=files[i];
		info[i].threadID=i;
		pthread_create(&threads[i],NULL,changeIndex,(void*)&info[i]);
	}
	for(i=10;i<NUMBER_OF_FILES;i++){
		pthread_join(threads[i],NULL);
	}

	// Now we delete files 0-9 in new-data directory
	for(i=0;i<10;i++){
		snprintf(path,49,"new-data/%d",i);
		remove(path);
	}

	// Now we rename files 10-49 to 0-39 in new-data directory
	for(i=10;i<50;i++){
		j=i-10;
		snprintf(path,49,"new-data/%d",i);
		snprintf(newpath,49,"new-data/%d",j);
		rename(path,newpath);
	}

	// Files 38 and 39 are now empty, so delete those
	remove("new-data/38");
	remove("new-data/39");

	// We weren't born in a barn...
	free(line);
	free(path);
	free(newpath);
	for(i=0;i<NUMBER_OF_FILES;i++){
		fclose(files[i]);
	}
	return 0;
}

// This method takes records from each file 1-9 and puts them in a file newFileNum
void InAFile(FILE * files[],int newFileNum){

	int i=0;
	int j=0;
	char * line=malloc(3001);
	for(i=0;i<10;i++){
		for(j=0;j<14;j++){
			if(fgets(line,3000,files[i])){
				fputs(line,files[newFileNum]);
			}
		}
	}
	free(line);
}

// This is our threaded method
void * changeIndex(void * info){
	threadInfo * ti=info;
	FILE * file =ti->file;
	FILE * newFile;
	char * temp = malloc(1000);
	char * temp2;
	char * indexString=malloc(50);
	char * filePath=malloc(50);
	int index=0;

	snprintf(filePath,49,"new-data/%d",ti->threadID);

	if(!(newFile=fopen(filePath,"w+"))) err_sys("Could not create file in thread %d",ti->threadID);

	rewind(file);
	while(fgets(temp,999,file)){
		temp2=strrchr(temp,','); // Find the last comma in the string
		temp2++;		 // This is the location of our index
		index=atoi(temp2);	 // Turn it into an integer
		index=(((index*9112)+7843)/517);	// Do some math
		snprintf(indexString,49,"%d\n",index);
		if(index==15)strcpy(indexString,"Number\n");	// Special case to fix a bug
		strncpy(temp2,indexString,49);
		fputs(temp,newFile);
	}
	free(temp);
	free(indexString);
	free(filePath);
	fclose(newFile);
	pthread_exit(NULL);
	return 0;
}
