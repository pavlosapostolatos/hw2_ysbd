#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HP.h"

int main(){
    BF_Init();
    if ((HP_CreateFile("test_hp",'i',"kleidi",sizeof(int))) < 0) {
        printf("Error creating file\n");
        return -1;
    }
    HP_info* file_info=HP_OpenFile("test_hp");
    if (file_info == NULL) {
        printf("Error opening file\n");
        return -1;
    }
    //epelekse poio arxeio me eggrafes thes na anoiksei
    FILE* f=fopen("records1K.txt","r");
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
        if (HP_InsertEntry(*file_info,r) < 0) {
            printf("Error inserting entry\n");
            return -1;
        }
    }
    int del=100;
    HP_DeleteEntry(*file_info,&del);
    del=999;
    HP_DeleteEntry(*file_info,&del);
    del=200;
    HP_DeleteEntry(*file_info,&del);
    del=998;
    HP_DeleteEntry(*file_info,&del);
    del=70;
    HP_DeleteEntry(*file_info,&del);
    del=1;
    HP_DeleteEntry(*file_info,&del);
    del=12;
    HP_DeleteEntry(*file_info,&del);
    del=30;
    HP_DeleteEntry(*file_info,&del);
    del=101;
    HP_DeleteEntry(*file_info,&del);
    //H 10000 den yparxei opote den svhnetai kati
    del=10000;
    HP_DeleteEntry(*file_info,&del);

    //To parakatw sxoliasmeno tmhma ektypwnei brute-force ola ta block tou arxeiou me th seira ama thelete na deite pws einai organwmena
    //void* block,*temp;
    // for(int m=1 ; m <= BF_GetBlockCounter(file_info->fileDesc) - 1 ; m++){
    //     BF_ReadBlock(file_info->fileDesc, m, &block);
    //     for (int j=0; j < BLOCK_SIZE-sizeof(Record); j+= sizeof(Record)) {
    //         temp=block+j;
    //         print_Record( (Record*) temp);
    //     }
    // }

    
    HP_GetAllEntries(*file_info,NULL);
    

    if (HP_CloseFile(file_info) < 0) {
        printf("Error closing file\n");
        return -1;
    }
    fclose(f);
}