#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include <assert.h>
/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

char *readJsonFile(){
		FILE *data =NULL;
		data =fopen("data.json", "r");
		assert(data!=NULL);
		char line[255];
		char *mystring=(char*)malloc(sizeof(char)*255);
		while(1){
			fgets(line, 255, data);
			if(feof(data)) break;
			mystring=(char*)realloc(mystring, strlen(mystring)+strlen(line)+1);
			strcat(mystring, line);
		}
		//printf("%s", mystring);
		fclose(data);
		return mystring;
}
void jsonNameList(const char *jsonstr, jsmntok_t *t, int tokcount, int *nameList){
	int count=0, i;
	for(i=0; i<tokcount; i++){
		if(t[i].size>0&&t[i].type==JSMN_STRING){
			nameList[count]=i;
			count++;
		}
	}
}

void printNames(const char *JSON_STRING, jsmntok_t *t, int *nameList){
	printf("****name list ****\n");
	int i=0;
	while(nameList[i]!=0){
		printf("[--NAME %2d] %.*s \n", i+1, t[nameList[i]].end-t[nameList[i]].start,JSON_STRING + t[nameList[i]].start);
		i++;
	}
}

void selectNameList(const char *JSON_STRING, jsmntok_t *t, int *nameList){
	int num=0;
	do{
		printf("Select Name's Number (0 to exit): ");
		int tester=scanf("%d", &num);
		if(tester==0||tester==EOF){
			printf("pls input number\n");
			char stringerror[20];
			scanf("%s", stringerror);
		}else if(num==0) ;
		else if(nameList[num-1]==0||num<0){
			printf("wrong index number\n");
		}else{
			int tokindex=nameList[num-1];
			printf("[--NAME %2d] %.*s \n", num, t[tokindex].end-t[tokindex].start,JSON_STRING + t[tokindex].start);
			printf("%.*s \n\n", t[tokindex+1].end-t[tokindex+1].start, JSON_STRING + t[tokindex+1].start);
		}
	}while(num!=0);
	printf("parser terminated\n");
}

void printtoken(const char *JSON_STRING, jsmntok_t *t, int count){
	int i=0;
	for(i=0; i<count; i++){
	printf("index: [%2d] start&end{%d, %d} size <%d> type '%d' %.*s\n", i, t[i].start, t[i].end, t[i].size, t[i].type, t[i].end-t[i].start,
			JSON_STRING + t[i].start);
		}
}
int main() {
	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128];
	int nameList[100]={0};

	const char *JSON_STRING =	readJsonFile();

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	jsonNameList(JSON_STRING, t, r, nameList);
	printNames(JSON_STRING,t, nameList);
	selectNameList(JSON_STRING, t, nameList);
	//printtoken(JSON_STRING, t, r);
	return EXIT_SUCCESS;
}
