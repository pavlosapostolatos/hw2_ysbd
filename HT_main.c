#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"

int main(){
    BF_Init();
    if ((HT_CreateIndex("test_ht",'i',"kleidi",sizeof(int),5000)) < 0) {
        printf("Error creating file\n");
        return -1;
    }
    HT_info* file_info=HT_OpenIndex("test_ht");
    if (file_info == NULL) {
        printf("Error opening file\n");
        return -1;
    }
    //epelekse poio arxeio me eggrafes thes na anoiksei
    FILE* f=fopen("records15K.txt","r");
    char row[100];
    char* ptr;
    Record r;
    //edw elegxoume kathe grammh tou arxeiou kai grafoume ta stoixeia sth domh record kai thn vazoume sto arxeio
    while(fscanf(f,"%s",row)!=EOF){
        //ptr=strtok(NULL,'{');
        ptr=strtok(row,",");
        r.id=atoi(ptr+1);
        ptr=strtok(NULL,",");
        ptr[strlen(ptr)-1]='\0';
        strcpy(r.name,ptr+1);
        ptr=strtok(NULL,",");
        ptr[strlen(ptr)-1]='\0';
        strcpy(r.surname,ptr+1);
        ptr=strtok(NULL,",");
        ptr[strlen(ptr)-1]='\0';
        ptr[strlen(ptr)-1]='\0';
        strcpy(r.address,ptr+1);
        if (HT_InsertEntry(*file_info,r) < 0) {
            printf("Error inserting entry\n");
            return -1;
        }
    }
    HashStatistics("test_ht");

    //Svhsimo twn eggrafwn me id 0-400
    for(int i=0;i<=400;i++){
        HT_DeleteEntry(*file_info,&i);
    }
    
    //Ektypwsh 10 diagegramenwn eggrafwn
    for(int j=0; j < 100 ; j=j+10)
        HT_GetAllEntries(*file_info,&j);

    //Ektypwsh 10 eggrafwn pou yparxoun mesa sto arxeio
    for(int k=500; k < 550 ; k=k+5)
        HT_GetAllEntries(*file_info,&k);

    // fclose(f);
    // f=fopen("records15K.txt","r");
    // //edw elegxoume kathe grammh tou arxeiou kai grafoume ta stoixeia sth domh record kai thn vazoume sto arxeio
    // while(fscanf(f,"%s",row)!=EOF){
    //     //ptr=strtok(NULL,'{');
    //     ptr=strtok(row,",");
    //     r.id=atoi(ptr+1);
    //     if (r.id > 400)
    //         break;
    //     ptr=strtok(NULL,",");
    //     ptr[strlen(ptr)-1]='\0';
    //     strcpy(r.name,ptr+1);
    //     ptr=strtok(NULL,",");
    //     ptr[strlen(ptr)-1]='\0';
    //     strcpy(r.surname,ptr+1);
    //     ptr=strtok(NULL,",");
    //     ptr[strlen(ptr)-1]='\0';
    //     ptr[strlen(ptr)-1]='\0';
    //     strcpy(r.address,ptr+1);
    //     if (HT_InsertEntry(*file_info,r) < 0) {
    //         printf("Error inserting entry\n");
    //     }
    // }

    if (HT_CloseIndex(file_info) < 0) {
        printf("Error closing file\n");
        return -1;
    }
    fclose(f);
}