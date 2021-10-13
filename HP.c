#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "HP.h"

int HP_CreateFile(char* fileName, char attrType,char* attrName, int attrLength ) {
    void* block;
    //make file
    //BF_Init();
    if (BF_CreateFile(fileName) < 0) {
		BF_PrintError("Error creating file");
		return -1;
	}
    //open file
    int file_id=0;
    if ( (file_id = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
        return -1;
    }
    //allocate info block
    if(BF_AllocateBlock(file_id) <0){
        BF_PrintError("Error allocating info block");
        return -1;
    }
    //read block
    if (BF_ReadBlock(file_id, 0, &block) < 0){
		BF_PrintError("Error getting block");
        return -1;
    }

    //edwgrafoume thn plhroforia tou file sto block 0
    HP_info info;
    info.fileDesc=file_id;
    info.attrType=attrType;
    info.attrLength=attrLength;
    strcpy(info.attrName,attrName);
    memcpy(block, &info, sizeof(HP_info));

    //write info block into memory
    if(BF_WriteBlock(file_id,BF_GetBlockCounter(file_id) - 1) <0){
        BF_PrintError("Error writing block");
        return -1;
    }  
    //close file
    if (BF_CloseFile(file_id) < 0) {
		BF_PrintError("Error closing file");
        return -1;
    }
    return 0;
}

HP_info* HP_OpenFile(char *fileName) {
    void* block;
    //open file
    int file_id=0;
    if ((file_id = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file");
        return NULL;
    }
    //diavazoume to block 0 pou einai oi plhrofories tou file
    if (BF_ReadBlock(file_id, 0, &block) < 0){
		BF_PrintError("Error getting block");
        return NULL;
    }
    HP_info* info=(HP_info*) malloc(sizeof(HP_info));
    memcpy(info, block, sizeof(HP_info));
    return info;
}

int HP_CloseFile(HP_info* header_info ) {
    int file_id=header_info->fileDesc;
    if (BF_CloseFile(file_id) < 0) {
		BF_PrintError("Error closing file");
        free(header_info);
        return -1;
    }
    free(header_info);
    return 0;
}

int HP_InsertEntry(HP_info header_info,Record record) {
    //edw elegxoume an h eggrafh yparxei hdh mesa sto arxeio
    if (HP_GetAllEntries(header_info,&(record.id))!=-1) { 
        printf("That record already exists\n");
        return -1;
    }
    void* block;
    int file_id=header_info.fileDesc;
    int block_counter= BF_GetBlockCounter(file_id) - 1;
    if (BF_ReadBlock(file_id, BF_GetBlockCounter(file_id) - 1, &block) < 0){
		BF_PrintError("Error getting block");
        return -1;
    }
    int flag=FALSE;
    if(block_counter!=0){
        void* record_counter=malloc(sizeof(int));
        //Edw pairnoume thn plhroforia gia to poses eggrafes exei mesa to block apo ta teleftea 4 byte
        memcpy(record_counter,block+(BLOCK_SIZE-4),sizeof(int));
        int record_counter_2 = (*((int*)record_counter));
        //edw vazoume thn eggrafh sto block
        if (record_counter_2 < (int)((BLOCK_SIZE-sizeof(int))/sizeof(Record)) ){
            memcpy(block+record_counter_2*sizeof(Record),&record, sizeof(Record));
            flag=TRUE;
            //edw afksanoume ton metrhth twn eggrafwn tou block kata 1
            record_counter_2++;
            memcpy(block+(BLOCK_SIZE-4),&record_counter_2,sizeof(int));
        }
        free(record_counter);        
    }
    //an ola ta blocks einai gemata den ginetai eisagwgh ston panw kwdika kai to flag paramenei false opote prepei na kanoume allocate kainourio block kai na th valoume ekei
    if(flag==FALSE){
        if(BF_AllocateBlock(file_id) <0){
            BF_PrintError("Error allocating block");
            return -1;
        }
        if (BF_ReadBlock(file_id, BF_GetBlockCounter(file_id) - 1, &block) < 0){
		    BF_PrintError("Error getting block");
            return -1;
        }
        memcpy(block,&record, sizeof(Record));
        //Edw grafoume sta teleftea 4 bytes tou block poses eggrafes exei mesa
        int rec_counter=1;
        memcpy(block+(BLOCK_SIZE-4),&rec_counter,sizeof(int));
        //edw arxikopoioume me NULL ton deikth sto epomeno block
        int number=-1;
        memcpy(block+(BLOCK_SIZE-8),&number,sizeof(int));
    }
    if(BF_WriteBlock(file_id,BF_GetBlockCounter(file_id) - 1) <0){
        BF_PrintError("Error writing block");
        return -1;
    }
    //Edw allazoume ton deikth tou telefteou block na deixnei sto kainourio block pou kaname allocate
    if (flag==FALSE){
        if (BF_ReadBlock(file_id, BF_GetBlockCounter(file_id) - 2, &block) < 0){
	        BF_PrintError("Error getting block");
            return -1;
        }
        int number=BF_GetBlockCounter(file_id) - 1;
        memcpy(block+(BLOCK_SIZE-8),&number,sizeof(int));
        if(BF_WriteBlock(file_id,BF_GetBlockCounter(file_id) - 2) <0){
            BF_PrintError("Error writing block");
            return -1;
        }
    }
    return BF_GetBlockCounter(file_id) - 1;
}

int HP_DeleteEntry(HP_info header_info,void *value) {
    int file_id=header_info.fileDesc,last_block;
    int i,j,flag=FALSE,record_counter_2;
    void* block,*temp,*del;
    void* record_counter=malloc(sizeof(int));
    void* last=malloc(sizeof(Record));
    //Ayto to loop xrhsimopoieitai gia na vroume to telefteo record kai na to antigrapsoume meta sthn thesi tou record pou tha diagrapsoume
    //Auto to kanoume gia na mhn exoume keno xwro axrhsimopoihto mesa sto arxeio mas
    for(j=BF_GetBlockCounter(file_id) - 1 ; j >=1 ; j--){
        //read block
        if (BF_ReadBlock(file_id, j, &block) < 0){
		    BF_PrintError("Error getting block");
            return -1;
        }
        //Edw pairnoume thn plhroforia gia to poses eggrafes exei mesa to block apo ta teleftea 4 byte
        memcpy(record_counter,block+(BLOCK_SIZE-4),sizeof(int));
        record_counter_2 = (*((int*)record_counter));
        if (record_counter_2==0)
            continue;
        memcpy(last,block+(record_counter_2-1)*sizeof(Record),sizeof(Record));
        //edw meiwnoume ton counter twn eggrafwn tou telefteou block afou ta mazevoume pros ta panw
        record_counter_2--;
        memcpy(block+(BLOCK_SIZE-4),&record_counter_2,sizeof(int));
        last_block=j;
        break; 
    }
    //Afou vrikame thn teleftea eggrafh pame kai thn antigrafoume se auth pou theloume na svhsoume kai diagrafoume th teleftea
    for (i=1 ; i <= BF_GetBlockCounter(file_id) - 1 && flag==FALSE ; i++){
        //read block
        if (BF_ReadBlock(file_id, i, &block) < 0){
		    BF_PrintError("Error getting block");
            return -1;
        }
        for (j=0; j < BLOCK_SIZE-sizeof(Record)-sizeof(int) && flag==FALSE; j+= sizeof(Record)){
            temp=block+j;
            //edw kanoume to mazema pros ta panw dhladh antigrafoume thn teleftea eggrafi tou file ekei pou svisame kai meta svhnoume thne teleftea eggrafh
            if (memcmp ( (  & (((Record*)temp)->id) ) ,value, sizeof(int))==0){
                memcpy(temp,last,sizeof(Record));
                //edw grafoume sth mnhmh to block pou egine h antikatastash
                if(BF_WriteBlock(file_id,i) <0){
                    BF_PrintError("Error writing block");
                    return -1;
                }
                flag=TRUE;
                break;
            }        
        }
    }
    //edw svinoume thn teleftea eggrafh apo to telefteo block
    if (flag==TRUE){
        if (BF_ReadBlock(file_id, last_block, &block) < 0){
		    BF_PrintError("Error getting block");
            return -1;
        }
        del=block+(record_counter_2)*sizeof(Record);
        memset(del,0, sizeof(Record));
        if(BF_WriteBlock(file_id,last_block) <0){
            BF_PrintError("Error writing block");
            return -1;
        }
    }
    free(record_counter);
    free(last);
    if(flag==TRUE) {
        printf("value deleted\n");
        return 0;
    }
    else
        return -1; 
}

int HP_GetAllEntries(HP_info header_info, void *value) {
    int file_id=header_info.fileDesc;
    int i,j,flag=FALSE;
    void* block,*temp,*zeros;
    zeros=malloc(sizeof(Record));
    memset(zeros,0, sizeof(Record));
    //afto to loop vlepei ena ena ta block
    for (i=1 ; i <= BF_GetBlockCounter(file_id) - 1 ; i++){
        //read block
        if (BF_ReadBlock(file_id, i, &block) < 0){
		    BF_PrintError("Error getting block");
            return -1;
        }
        //afto to loop vlepei mia mia tis eggrafes
        for (j=0; j < BLOCK_SIZE-sizeof(Record)-sizeof(int); j+= sizeof(Record)) {
            temp=block+j;
            //an to value einai NULL ektypwnoume oles tis eggrafes tou arxeiou
            if (value==NULL && memcmp(temp,zeros, sizeof(Record))!=0){
                print_Record((Record*)temp);
                flag=TRUE;
            }
            //otan vrei mia eggrafh me id=value thn ektypwnei
            //h deuterh synthiki tou if einai eidiki periptwsh giati an h eggrafh exei id=0 thn mperdevei me tis diagegramenes
            else if (((value==NULL) || memcmp ( (  & (((Record*)temp)->id) ) ,value, sizeof(int))==0) && memcmp(temp,zeros, sizeof(Record))!=0 ){
                print_Record((Record*)temp);
                flag=TRUE;
            }        
        }
    }
    free(zeros);
    if(flag==FALSE) {
        return -1;
    }
    return i-1;
}

//auth einai mia voithitikh synarthsh gia na ektypwnei tis eggrafes
void print_Record(Record * r){
    printf("record with id %d: %s,%s,%s \n",r->id,r->name,r->surname,r->address);
}