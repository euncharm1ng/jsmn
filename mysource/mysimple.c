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
void jsonNameList(const char *jsonstr, jsmntok_t *t, int tokcount){
	char nameList[100][20];
	int count=0;

	int i;
	for(i=0; i<tokcount; i++){
		if(t[i].size>0&&t[i].type==JSMN_STRING){
			strncpy(nameList[count],jsonstr + t[i].start, t[i].end-t[i].start );
			count++;
		}
	}
	printf("****name list ****\n");
	for(i=0; i<count; i++){
		printf("[NAME %2d] %s \n", i+1, nameList[i]);
	}
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
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	const char *JSON_STRING =	readJsonFile();

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}
	//jsonNameList(JSON_STRING, t, r);
	//printtoken(JSON_STRING, t, r);
	/* Loop over all keys of the root object */

	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "name") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- name: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "keywords") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- keywords: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "description") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "examples") == 0) {
			int j;
			printf("- examples:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i+1].size + 1;
		} else {
			//printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
				//	JSON_STRING + t[i].start);
		}
	}
	jsonNameList(JSON_STRING, t, r);
	//printtoken(JSON_STRING, t, r);
	return EXIT_SUCCESS;
}
