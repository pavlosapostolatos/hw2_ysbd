#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"

#define TRUE 1
#define FALSE 0

typedef struct {
char fileName[20];
int fileDesc; 
char attrType;
char attrName[10];
int attrLength;
int numBuckets;
} HT_info; 

typedef struct{
int id;
char name[15];
char surname[25];
char address[50];
}Record; 

int HT_CreateIndex(char*, char,char*, int,int);
HT_info* HT_OpenIndex(char *);
int HT_InsertEntry(HT_info,Record);
int HT_CloseIndex(HT_info*);
int HT_GetAllEntries(HT_info, void *);
int HT_DeleteEntry( HT_info header_info,void *);
int HashStatistics(char* filename);
int h(int,int);
void print_Record(Record *);