/*
 * main.c
 *
 *  Created on: 22.03.2020
 *      Author: Felix Erckenbrecht
 *
 *
 *  Patch Python Framework Path in Civ IV executable for macOS / OSX
 *  to point to /Library/Frameworks instead of /System/Library/Frameworks
 *
 *  This enables deploying the required Python 2.3 framework without
 *  having to disable SIP.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char * * argv){
	FILE * f;
	char * buf;
	off_t filesize;
	off_t bytes;
	char * p;
	char needle[]="/System/Library/Frameworks/Python.framework/Versions/2.3/Python";
	const char patched[]="/Library/Frameworks/Python.framework/Versions/2.3/Python";
	int i;

	if(argc != 2){
		printf( "Usage:\n"
				"civ4ppp <path to civ4 executable>\n");
		exit(-1);
	}
	f = fopen(argv[1],"r+");
	if(f == NULL){
		perror("Error opening file.");
		exit(-1);
	}
	if(fseek(f,0,SEEK_END)<0){
		perror("");
		exit(-1);
	}
	filesize = ftell(f);
	fseek(f,0L,SEEK_SET);

	if(filesize > 1024*1024*100){
		printf("Filesize exceeds 100M, expected ~83 MB.");
		exit(-1);
	}
	buf = malloc(filesize);
	if(buf==NULL){
		printf("Error allocation data buffer.\n");
		exit(-1);
	}
	bytes = fread(buf, 1, filesize, f);
	if(bytes != filesize){
		printf("Could not read entire file.\n");
		exit(-1);
	}
	p = buf;
	i = 0;
	do{
		p = memmem(p,p - buf,patched,strlen(patched)+1);
		if(p != NULL){
			i++;
			p++;
		}
	}while(p!=NULL);
	if(i == 2){
		printf("File already patched.\n");
		exit(0);
	}

	p = buf;
	i = 0;
	do{
		p = memmem(p,bytes - (p-buf),needle,strlen(needle)+1);
		if(p != NULL){
			i++;
			p++;
		}
	}while(p!=NULL);

	if(i == 0){
		printf("No python path entries found. Nothing to patch.\n");
		exit(-1);
	}

	if(i > 2){
		printf("Found more occurrences (%d) than expected (2).\n",i);
	}

	if(i == 1){
		printf("Found only one occurrence, expected 2. Still patching...\n");
	}
	else{
		printf("Found 2 occurrences, patching.\n");
	}
	p = buf;
	i = 0;
	do{
		p = memmem(p,bytes - (p-buf),needle,strlen(needle)+1);
		if(p != NULL){
			off_t filepos;
			size_t written;

			if(snprintf(p,strlen(needle),patched) >= strlen(needle)){
				printf("Not enough room to patch. Aborting.\n");
				exit(-1);
			}
			filepos = p-buf;
			printf("Pos: %zu\n", (size_t) filepos);
			if(fseek(f, filepos ,SEEK_SET)){
				perror("Error seeking.");
				exit(-1);
			}
			written = fwrite(patched, 1, strlen(patched)+1, f);
			if( written != strlen(patched)+1){
				printf("Error writing to file at %zu (%zu).\nAborting.\n",(size_t) filepos,written);
				printf("ferror: %d",ferror(f));
				perror("");
				exit(-1);
			}
			p++;
			i++;
		}
	}while(p!=NULL);
	fclose(f);
	printf("Patched %d occurrences.\n",i);
	return(0);
}
