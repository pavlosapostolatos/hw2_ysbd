#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "SHT.h"

int main(){
    BF_Init();
    srand(time(NULL));
    if ((HT_CreateIndex("test_ht",'i',"kleidi",sizeof(int),200)) < 0) {
        printf("Error creating file\n");
        return -1;
    }
    HT_info* file_info=HT_OpenIndex("test_ht");
    if (file_info == NULL) {
        printf("Error opening file\n");
        return -1;
    }
    if ((SHT_CreateSecondaryIndex("stest_ht","kleidi",sizeof(int),200,"test_ht")) < 0) {
        printf("Error creating file\n");
        return -1;
    }
    SHT_info* sfile_info=SHT_OpenSecondaryIndex("stest_ht");
    if (sfile_info == NULL) {
        printf("Error opening file\n");
        return -1;
    }
    //epelekse poio arxeio me eggrafes thes na anoiksei
    FILE* f=fopen("records10K.txt","r");
    char row[100];
    int block_num;
    char* ptr;
    Record r;
    SecondaryRecord sr;
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
        if ((block_num=HT_InsertEntry(*file_info,r) )< 0) {
            printf("Error inserting entry\n");
            return -1;
        }
        sr.block_id=block_num;
        sr.record=r;
        if (SHT_SecondaryInsertEntry(*sfile_info,sr) < 0) {
            printf("Error inserting entry\n");
            return -1;
        }
    }
    char str[20];
    char str2[20];
    //Ektypwsh 500 tyxaiwn epwnymwn apo to arxeio
    for(int k=0; k <= 500 ; k++){
        strcpy(str,"surname_");
        sprintf(str2,"%d",rand()%10000);
        strcat(str,str2);
        //HT_GetAllEntries(*file_info,&k);
        SHT_SecondaryGetAllEntries(*sfile_info,*file_info,str);
    }

    SecondaryHashStatistics("stest_ht");

    // //To parakatw sxoliasmeno tmhma ektypwnei brute-force ola ta block tou arxeiou me th seira ama thelete na deite pws einai organwmena
    // void* block,*temp;
    // for(int m=2 ; m <= BF_GetBlockCounter(sfile_info->fileDesc) - 1 ; m++){
    //     BF_ReadBlock(sfile_info->fileDesc, m, &block);
    //     for (int j=0; j < BLOCK_SIZE-sizeof(SecondaryRecord); j+= sizeof(SecondaryRecord)) {
    //         temp=block+j;
    //         print_Record(&(((SecondaryRecord*) temp)->record));
    //         printf("%d\n",((SecondaryRecord*) temp)->block_id);
    //     }
    // }

    fclose(f);
    if (SHT_CloseSecondaryIndex(sfile_info) < 0) {
        printf("Error closing file\n");
        return -1;
    }
    if (HT_CloseIndex(file_info) < 0) {
        printf("Error closing file\n");
        return -1;
    }
}