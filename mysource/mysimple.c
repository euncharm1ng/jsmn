#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include <assert.h>

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
		fclose(data);
		return mystring;
}
void jsonNameList(const char *jsonstr, jsmntok_t *t, int tokcount, int *nameList){
	int count=0, i, counta=0;
	for(i=0; i<tokcount; i++){
		if(t[i].size>0&&t[i].type==JSMN_STRING){
			nameList=(int*)realloc(nameList, sizeof(int)*(count+2));
			nameList[count]=i;
			count++;
		}
	}
	nameList[count]=0;
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
	printNames(JSON_STRING,t, nameList);
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


/* 입력 토큰을 받아서 토큰의 parent가 어레이rh 그 토큰의 타입이 오브젝트이면
	해당 토큰의 인덱스 번호를 저장하는 함수
	realloc으로 사이즈를 늘리기 때문에 에레이의 사이즈를 하나 크게 만들고 0을 넣는다
	(realloc가 default로 넣는 값이 0이 아니기 때문에)
	또한 새롭게 만든 어레이의 크기를 리턴한다
	(sizeof(objectList)/sizeof(objectList[0])이 정확한 값을 돌려주기 않기 때문에 ) */
int jsonObjectList(const char *jsonstr, jsmntok_t *t, int tokcount, int *objectList){
	int count=0, i;
	for(i=0; i<tokcount; i++){
		if(t[i].type==JSMN_OBJECT&&t[t[i].parent].type==JSMN_ARRAY){
			objectList=(int*)realloc(objectList, sizeof(int)*(count+2));
			objectList[count]=i;
			count++;
		}
	}
	objectList[count]=0;
	return count+1;
}


/*jsonObjectList()에서 만든 어레이를 받아서 안에 있는 토큰의 인덱스 번호를 받는다(시작위치)
	그리곤 시작 위치의 다음 nameList의 토큰의 value를 출력한다. */
void printObjects(const char *JSON_STRING, jsmntok_t *t, int length, int *objectList, int *nameList){
	printf("****Object List****\n");
	int i=0, j=0;
	while(objectList[i]!=0){
		while(nameList[j]<objectList[i]) j++;
		printf("[--NAME %2d] %.*s \n", i+1, t[nameList[j]+1].end-t[nameList[j]+1].start,JSON_STRING + t[nameList[j]+1].start);
		i++;
	}
}


/*공간 절약을 위해 objectList를 이 함수에서 만든다. jsonObjectList()로 오브젝트의 어레이를 만들고 어레이의 길이를 받는다.
	printObjects()로 메뉴(오브젝트의 첫번째 value)를 출력한다.
	유저가 문자를 입력하거나 틀린 인덱스 번호를 입력할 수 있는 경우를 한 번 걸러준다.
	그리곤 유저가 고른 오브젝트의 첫 토큰을 nameList안에서 찾는다
	만약 유저가 고른 오브젝트가 마지막 오브젝드가 아니면 다음 object가 나타날 때까지 토큰을 출력한다.
	만약 마지막 오브젝트를 선택했다면 끝나는 지점을 전체 토큰의 끝으로 하고
	nameList의 마지막 element인 0으로 while문을 끝낸다. */
void selectObjectList(const char *JSON_STRING, jsmntok_t *t, int *nameList, int tokcount){
	int *objectList=(int*)malloc(sizeof(int));
	int objLength=jsonObjectList(JSON_STRING, t, tokcount, objectList);
	printObjects(JSON_STRING, t, tokcount, objectList, nameList);

	int num=0,i=0;
	do{
		printf("Select object's Number (0 to exit): ");
		int tester=scanf("%d", &num);
		if(tester==0||tester==EOF){
			printf("pls input number\n");
			char stringerror[20];
			scanf("%s", stringerror);
		}else if(num==0) ;
		else if(objLength<=num||num<0){
			printf("wrong index number\n");
		}else{
			i=0;
			while(objectList[num-1]>nameList[i]) i++; //유저가 고른 오브젝트의 첫 토큰을 nameList안에서 찾는다
			int endPoint = (objectList[num]==0)? tokcount: objectList[num];
			printf("%.*s : ", t[nameList[i]].end-t[nameList[i]].start,JSON_STRING + t[nameList[i]].start);
			printf("%.*s\n", t[nameList[i]+1].end-t[nameList[i]+1].start, JSON_STRING + t[nameList[i]+1].start);
			int tokindex=nameList[++i];
			while(tokindex<endPoint&&tokindex!=0){
				printf("\t[%.*s]", t[tokindex].end-t[tokindex].start,JSON_STRING + t[tokindex].start);
				printf(" %.*s\n", t[tokindex+1].end-t[tokindex+1].start, JSON_STRING + t[tokindex+1].start);
				tokindex=nameList[++i];
			}
		}
	}while(num!=0);
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
	int *nameList=(int*)malloc(sizeof(int));

	const char *JSON_STRING =	readJsonFile();

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}
	jsonNameList(JSON_STRING, t, r, nameList);
	//selectNameList(JSON_STRING, t, nameList);
	selectObjectList(JSON_STRING, t, nameList, r);
	//t[0].type==JSMN_OBJECT? selectNameList(JSON_STRING, t, nameList): selectObjectList(JSON_STRING, t, objectList, nameList, r);
	//printtoken(JSON_STRING, t, r);
	return EXIT_SUCCESS;
}
