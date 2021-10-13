#include <stdio.h>

#include "BF.h"

#define TRUE 1
#define FALSE 0

typedef struct {
int fileDesc; 
char attrType;
char attrName[10];
int attrLength;
} HP_info; 

typedef struct{
int id;
char name[15];
char surname[25];
char address[50];
}Record;

int HP_CreateFile(char*, char,char*, int);
HP_info* HP_OpenFile(char *);
int HP_CloseFile(HP_info*);
int HP_InsertEntry(HP_info,Record);
int HP_DeleteEntry(HP_info,void *);
int HP_GetAllEntries(HP_info, void *);
void print_Record(Record *);