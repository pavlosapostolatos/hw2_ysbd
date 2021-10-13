#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "HT.h"

#define TRUE 1
#define FALSE 0

typedef struct {
    char filename[20];
    int fileDesc; 
    char attrName[10];
    int attrLength;
    long int numBuckets;
} SHT_info; 

typedef struct{
    Record record;
    int block_id;
}SecondaryRecord; 

int SHT_CreateSecondaryIndex(char*, char*,int,int,char*);
SHT_info* SHT_OpenSecondaryIndex(char *filename);
int SHT_CloseSecondaryIndex(SHT_info*);
int SHT_SecondaryInsertEntry( SHT_info,SecondaryRecord);
int SHT_SecondaryGetAllEntries(SHT_info,HT_info,void *);
int SecondaryHashStatistics(char*);
unsigned int str_hash(char *str,int);