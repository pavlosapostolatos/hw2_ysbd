#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "HT.h"

int HT_CreateIndex(char* fileName, char attrType,char* attrName, int attrLength,int buckets) {
    void* block;
    //make file
    //BF_Init();
    if (BF_CreateFile(fileName) < 0) {
		BF_PrintError("Error creating file\n");
		return -1;
	}
    //open file
    int file_id=0;
    if ( (file_id = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file\n");
        return -1;
    }
    //allocate info block
    if(BF_AllocateBlock(file_id) <0){
        BF_PrintError("Error allocating info block\n");
        return -1;
    }
    //read block
    if (BF_ReadBlock(file_id, 0, &block) < 0){
		BF_PrintError("Error getting block\n");
        return -1;
    }

    //edw grafoume thn plhroforia tou arxeio sto block 0 tou arxeiou
    HT_info info;
    info.fileDesc=file_id;
    info.attrType=attrType;
    info.attrLength=attrLength;
    info.numBuckets=buckets;
    strcpy(info.attrName,attrName);
    memcpy(block, &info, sizeof(HT_info));
    //write info block into memory
    if(BF_WriteBlock(file_id,BF_GetBlockCounter(file_id) - 1) <0){
        BF_PrintError("Error writing block\n");
        return -1;
    }

    //edw ftiaxnoume ton pinaka katakermatismou analoga me to posa block xreiazetai me vash to megethos tvn bucket tou
    int int_capacity=(BLOCK_SIZE-sizeof(int))/sizeof(int);
    int array_blocks= buckets/int_capacity;
    if (buckets%int_capacity!=0)
        array_blocks++;
    for(int i=1 ; i<=array_blocks ; i++){
        //allocate array block
        if(BF_AllocateBlock(file_id) <0){
            BF_PrintError("Error allocating info block\n");
            return -1;
        }
        //read block
        if (BF_ReadBlock(file_id,BF_GetBlockCounter(file_id) - 1, &block) < 0){
		    BF_PrintError("Error getting block\n");
            return -1;
        }
        //set array cells to null (-1)
        int null = -1;
        void* temp=block;
        for(int j=0 ; j < int_capacity ; j++){
            temp=block+j*sizeof(int);
            memcpy(temp,&null,sizeof(int));
        }
        if(BF_WriteBlock(file_id,BF_GetBlockCounter(file_id) - 1) <0){
            BF_PrintError("Error writing block\n");
            return -1;
        }
    }
    //close file
    if (BF_CloseFile(file_id) < 0) {
		BF_PrintError("Error closing file\n");
        return -1;
    }
    return 0;
}

HT_info* HT_OpenIndex(char *fileName) {
    void* block;
    //open file
    int file_id=0;
    if ((file_id = BF_OpenFile(fileName)) < 0) {
		BF_PrintError("Error opening file\n");
        return NULL;
    }
    //edw diavazoume to block 0 pou einai h plhrofories tou file
    if (BF_ReadBlock(file_id, 0, &block) < 0){
		BF_PrintError("Error getting block\n");
        return NULL;
    }
    HT_info* info=(HT_info*) malloc(sizeof(HT_info));
    memcpy(info, block, sizeof(HT_info));
    return info;
}

int HT_CloseIndex(HT_info* header_info ) {
    int file_id=header_info->fileDesc;
    if (BF_CloseFile(file_id) < 0) {
		BF_PrintError("Error closing file\n");
        free(header_info);
        return -1;
    }
    free(header_info);
    return 0;
}

int h(int key,int size) {    
	int h=0, a=33;
	while(key!=0){
		h=(a*h + key%10) % (size);
        key=key/10;
    }
	return h;
}

//auth einai mia voithitikh synarthsh gia na ektypwnei tis eggrafes
void print_Record(Record * r){
    printf("record with id %d: %s,%s,%s \n",r->id,r->name,r->surname,r->address);
}

int HT_InsertEntry(HT_info header_info, Record record) {
    void* block;
    void *empty_rec=NULL;
    void* zeros=malloc(sizeof(Record));
    memset(zeros,0,sizeof(Record));
    int prev_block;
    int empty_rec_block=-1;
    int file_id=header_info.fileDesc;
    int key=h(record.id,header_info.numBuckets);
    int int_capacity=(BLOCK_SIZE-sizeof(int))/sizeof(int);
    //edw pairnoume to block me to meros tou pinaka pou tha xreiastoume gia na valoume thn kainouria eggrafh
    int block_position=(key/int_capacity)+1;
    if (BF_ReadBlock(file_id, block_position, &block) < 0){
		BF_PrintError("Error getting block\n");
        return -1;
    }
    //edw epilegoume to bucket pou tha mpei h eggrafh analoga me thn hash function
    void* cell_position=block+(key%int_capacity)*sizeof(int);
    //auth h synthiki einai gia otan o pinakas exei keno keli se ekeino to shmeio opote kanoume allocate kainourio block
    if( (*(int*)cell_position)==-1  ){
        //allocate new block
        if(BF_AllocateBlock(file_id) <0){
            BF_PrintError("Error allocating info block\n");
            return -1;
        }
        //vazoume to kelii tou pinaka na deixnei sto kainourio block
        int number=BF_GetBlockCounter(file_id) - 1;
        memcpy(cell_position,&number,sizeof(int));
        if(BF_WriteBlock(file_id,block_position) <0){
            BF_PrintError("Error writing block\n");
            return -1;
        }
        if (BF_ReadBlock(file_id,BF_GetBlockCounter(file_id) - 1, &block) < 0){
		    BF_PrintError("Error getting block\n");
            return -1;
        }
        //arxikopoihsh tou record counter
        void* temp=block+(BLOCK_SIZE-4);
        number=1;
        memcpy(temp,&number,sizeof(int));
        //arxikopoihsh tou deikth sto epomeno block
        temp=block+(BLOCK_SIZE-8);
        number=-1;
        memcpy(temp,&number,sizeof(int));
        //vazoume sto block thn kainouria eggrafh
        memcpy(block+0,&record,sizeof(Record));
        if(BF_WriteBlock(file_id,BF_GetBlockCounter(file_id) - 1) <0){
            BF_PrintError("Error writing block\n");
            return -1;
        }
    }
    //an o pinakas den exei keno keli pame kai psaxnoume sto bucket seiriaka an yparxei hdh mesa h eggrafh gia na mhn thn valoume 2h fora
    //kai parallhla kratame kai enan deikth sthn prwth kenh thesi pou yparxei sto bucket
    else{
        void* temp=cell_position;
        int number=-1;
        //edw psaxnoume ola ta blocks tou bucket gia oles tis eggrafes gia na doume an yparxei hdh mesa h eggraf  pou theloume na prosthesoume
        while(memcmp(temp,&number,sizeof(int))!=0){
            if (BF_ReadBlock(file_id,(*(int*)temp), &block) < 0){
		        BF_PrintError("Error getting block\n");
                return -1;
            }
            for (int j=0; j < BLOCK_SIZE-sizeof(Record)-2*sizeof(int); j+= sizeof(Record)) {
                void* temp_2=block+j;
                //h deuterh synthiki tou if einai eidiki periptwsh giati an h eggrafh exei id=0 thn mperdevei me tis diagegramenes
                if (memcmp ( (  & (((Record*)temp_2)->id) ) ,&record.id, sizeof(int))==0 && memcmp(temp_2,zeros, sizeof(Record))!=0  ){
                    printf("This record already exists!\n");
                    return -1;
                }
                //edw vriskoume thn prwth kenh thesi sto bucket kai tin kratame gia na mpei meta ekei h eggrafh
                if((memcmp(temp_2,zeros,sizeof(Record))==0) && empty_rec==NULL){ 
                    empty_rec=temp_2;
                    empty_rec_block=(*(int*)temp);
                } 
            }
            prev_block=(*(int*)temp);
            temp=block+(BLOCK_SIZE-8);
        }
        //edw an to empty_rec!=NULL shmainei oti vrhkame kenh thesh kapou mesa sto bucket opote tha valoume thn eggrafh mas ekei
        if(empty_rec!=NULL){
            if (BF_ReadBlock(file_id,empty_rec_block, &block) < 0){
		        BF_PrintError("Error getting block\n");
                return -1;
            }
            memcpy(empty_rec,&record,sizeof(Record));
            temp=block+(BLOCK_SIZE-4);
            int record_counter=(*(int*)temp);
            record_counter++;
            memcpy(temp,&record_counter,sizeof(int));
            if(BF_WriteBlock(file_id,empty_rec_block) < 0){
                BF_PrintError("Error writing block\n");
                return -1;
            }
        }
        //an ftasoume edw shmainei oti olo to bucket einai gemato me records opote prepei na kanoume allocate kainourio block sto bucket
        else{
            if (BF_ReadBlock(file_id,prev_block, &block) < 0){
		        BF_PrintError("Error getting block\n");
                return -1;
            }
            //allocate new block
            if(BF_AllocateBlock(file_id) <0){
                BF_PrintError("Error allocating info block\n");
                return -1;
            }
            //arxikopoihsh tou deikth tou prohgoumeno block na deixnei se auto
            int number=BF_GetBlockCounter(file_id) - 1;
            memcpy(block+(BLOCK_SIZE-8),&number,sizeof(int));
            if(BF_WriteBlock(file_id,prev_block) < 0){
                BF_PrintError("Error writing block\n");
                return -1;
            }
            //vazoume sto kainourio block thn kainouria eggrafh
            if (BF_ReadBlock(file_id,BF_GetBlockCounter(file_id) - 1, &block) < 0){
		        BF_PrintError("Error getting block\n");
                return -1;
            }
            //arxikopoihsh tou record counter
            void* temp=block+(BLOCK_SIZE-4);
            number=1;
            memcpy(temp,&number,sizeof(int));
            //arxikopoihsh tou deikth sto epomeno block me NULL
            temp=block+(BLOCK_SIZE-8);
            number=-1;
            memcpy(temp,&number,sizeof(int));
            memcpy(block+0,&record,sizeof(Record));
            if(BF_WriteBlock(file_id,BF_GetBlockCounter(file_id) - 1) <0){
                BF_PrintError("Error writing block\n");
                return -1;
            }
        }  
    }
    free(zeros);
    if (empty_rec==NULL)
        return BF_GetBlockCounter(file_id) - 1;
    else
        return empty_rec_block; 
}

int HT_GetAllEntries(HT_info header_info, void* value){
    void* block;
    void* zeros=malloc(sizeof(Record));
    int id=(*(int*)value);
    int file_id=header_info.fileDesc;
    int key=h(id,header_info.numBuckets);
    int int_capacity=(BLOCK_SIZE-sizeof(int))/sizeof(int);
    //edw pairnoume to meros tou pinaka pou tha xreiastoume
    int block_position=(key/int_capacity)+1;
    if (BF_ReadBlock(file_id, block_position, &block) < 0){
		BF_PrintError("Error getting block\n");
        return -1;
    }
    //edw epilegoume to bucket pou tha psaksoume gia thn eggrafh
    void* cell_position=block+(key%int_capacity)*sizeof(int);
    void* temp=cell_position;
    int number=-1;
    int counter; //metritis gia to posa block diavasame mexri na vroume thn eggrafh
    while(memcmp(temp,&number,sizeof(int))!=0){
        counter++;
        if (BF_ReadBlock(file_id,(*(int*)temp), &block) < 0){
		    BF_PrintError("Error getting block\n");
            return -1;
        }
        for (int j=0; j < BLOCK_SIZE-sizeof(Record)-2*sizeof(int); j+= sizeof(Record)) {
            void* temp_2=block+j;
            //edw psaxnoume an h eggrafh yparxei hdh
            if (memcmp ( (  & (((Record*)temp_2)->id) ) ,value, sizeof(int))==0 && memcmp(temp_2,zeros, sizeof(Record))!=0  ){
                print_Record((Record*)temp_2);
                free(zeros);
                return counter;
            } 
        }
        temp=block+(BLOCK_SIZE-8);
    }
    printf("Record not found\n");
    free(zeros);
    return -1;
}

//Efoson h insert lamvanei ypopsin kenes theseis se ena bucket h delete to mono pou exei na kanei einai na diagrapsei thn eggrafh pou theloume kai tipota allo
int HT_DeleteEntry( HT_info header_info,void *value) {
    void* block;
    void* zeros=malloc(sizeof(Record));
    int id=(*(int*)value);
    int file_id=header_info.fileDesc;
    int key=h(id,header_info.numBuckets);
    int int_capacity=(BLOCK_SIZE-sizeof(int))/sizeof(int);
    //edw pairnoume to meros tou pinaka pou tha xreiastoume
    int block_position=(key/int_capacity)+1;
    if (BF_ReadBlock(file_id, block_position, &block) < 0){
		BF_PrintError("Error getting block\n");
        return -1;
    }
    //edw epilegoume to bucket pou tha psaksoume gia thn eggrafh
    void* cell_position=block+(key%int_capacity)*sizeof(int);
    void* temp=cell_position;
    int number=-1;
    //afto to while psaxnei ta blocks tou bucket
    while(memcmp(temp,&number,sizeof(int))!=0){
        if (BF_ReadBlock(file_id,(*(int*)temp), &block) < 0){
		    BF_PrintError("Error getting block\n");
            return -1;
        }
        //auth h for psaxnei tiw eggrafes twn block
        for (int j=0; j < BLOCK_SIZE-sizeof(Record)-2*sizeof(int); j+= sizeof(Record)) {
            void* temp_2=block+j;
            //edw psaxnoume thn eggrafh
            if (memcmp ( (  & (((Record*)temp_2)->id) ) ,value, sizeof(int))==0 && memcmp(temp_2,zeros, sizeof(Record))!=0  ){
                memset(temp_2,0,sizeof(Record));
                int record_counter=(*(int*)(block+(BLOCK_SIZE-4)));
                record_counter--;
                memcpy(block+(BLOCK_SIZE-4),&record_counter,sizeof(int));
                if(BF_WriteBlock(file_id,(*(int*)temp)) <0){
                    BF_PrintError("Error writing block\n");
                    return -1;
                }
                free(zeros);
                return 0;
            } 
        }
        temp=block+(BLOCK_SIZE-8);
    }
    //an ftasei to programma edw shmainei oti h eggrafh pou theloume na diagrapsoume den yparxei mesa
    printf("Record not found so it cannot be deleted\n");
    free(zeros);
    return -1;
}

int HashStatistics(char* filename){
    void* temp,*block,*block_2;
    HT_info* file_info;
    file_info=HT_OpenIndex(filename);
    int file_id=file_info->fileDesc;
    int array_buckets=file_info->numBuckets;
    printf("Blocks arxeiou:%d\n",BF_GetBlockCounter(file_id) - 1);
    int int_capacity=(BLOCK_SIZE-sizeof(int))/sizeof(int);
    int array_blocks= array_buckets/int_capacity;
    if (array_buckets%int_capacity!=0)
        array_blocks++;
    //edw einai oi metrites
    int min_rec=9999,max_rec=0;
    int bucket_block_counter=0,global_block_counter=0,rec_counter=0,global_rec_counter=0;
    //loop gia kathe meros tou pinaka (ana 127 kelia)
    for(int i=1 ; i<=array_blocks ; i++){
        //auth h for diatrexei ta kelia tou pinaka
        for(int j=0 ; j < int_capacity ; j++){
            int number=-1;
            bucket_block_counter=0;
            rec_counter=0;
            if (BF_ReadBlock(file_id,i, &block) < 0){
		        BF_PrintError("Error getting block");
                return -1;
            }
            temp=block+j*sizeof(int);
            //afto to while diatrexei ta blocks tou bucket
            while(memcmp(temp,&number,sizeof(int))!=0){
                //edw me thn bucket block_counter metrame ta blocks tou kathe bucket
                bucket_block_counter++;
                //edw me thn global_block_counter metrame to synolo twn block olwn twn bucket
                global_block_counter++;
                if (BF_ReadBlock(file_id,(*(int*)temp), &block_2) < 0){
		            BF_PrintError("Error getting block");
                    return -1;
                }
                //auth h for diatrexei tis eggrafes twn block
                for (int k=0; k < BLOCK_SIZE-sizeof(Record)-2*sizeof(int); k+= sizeof(Record)) {
                    //edw metrame ta records tou kathe bucket
                    rec_counter++;
                }
                temp=block_2+(BLOCK_SIZE-8);
            }
            //Edw ginetai h ektypwsh twn block ypereilhshs tou kathe bucket
            if(bucket_block_counter > 1)
                printf("Bucket number %d: Has %d overflow blocks\n",int_capacity*(i-1)+j,bucket_block_counter-1);
            //Edw enhmerwnoume to elaxisto plhthwn eggrafwn bucket an einai aparaithto
            if(rec_counter < min_rec)
                min_rec=rec_counter;
            //Edw enhmerwnoume to megisto plhthwn eggrafwn bucket an einai aparaithto
            if(rec_counter > max_rec)
                max_rec=rec_counter;
            //edw auksanoume ton metrhth twn eggrafwn pou yparxoun se olo to arxeio prosthetontas tou tis eggrafes tou bucket pou molis psaksame
            global_rec_counter+=rec_counter;
        }
    }
    printf("Minimum records from all buckets: %d\n",min_rec);
    printf("Maximum records from all buckets: %d\n",max_rec);
    printf("Average records in buckets: %d\n",global_rec_counter/file_info->numBuckets);
    printf("Average blocks in buckets: %d\n",global_block_counter/file_info->numBuckets);
    return 0;
}