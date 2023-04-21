#include "cpmfsys.h"
#include "diskSimulator.h"

bool freeList[NUM_BLOCKS];

// copy contents of extent (index) from block 0 pointer (e)
DirStructType *mkDirStruct(int index, uint8_t *e)
{
    // retrieve mem addr of desired extent
    uint8_t *dir_addr = (e + index * EXTENT_SIZE);

    // create a new pointer to a DirStructType named "d" and allocate memory
    DirStructType *d = (DirStructType *)malloc(sizeof(DirStructType));

    // copy status
    d->status = dir_addr[0]; // status is only 1 byte

    // copy name + null terminator
    int name_index; // index for the name
    char name_letter; // temp storage for the letter at that index
    for(name_index = 0; name_index<8; name_index++){
        name_letter = dir_addr[name_index+1]; // store the letter found
        // check if the char is a space character (32 in ASCII)
        if( name_letter != 32){
            // printf("%c\n",letter); // print the letter
            d->name[name_index] = name_letter; // store the letter
        }
        else{
            break;
        }
    }
    d->name[name_index+1] = '\0'; // add null terminator
    // printf("%s\n", d->name); // print the stored name

    // copy extension + null terminator
    int ext_index; // index for ext
    char ext_letter; // temp storage for the letter at that index
    for(ext_index=0; ext_index<3; ext_index++){
        ext_letter = dir_addr[ext_index+9]; // store the letter found
        // check if the char is a space character (32 in ASCII)
        if( ext_letter != 32){
            // printf("%c\n", letter); // print the letter
            d->extension[ext_index] = ext_letter;
        }
        else{
            break;
        }
    }
    d->extension[ext_index+1] = '\0'; // add null terminator
    // printf("%s\n", d->extension); // print the stored name

    // copy BC
    d->BC = dir_addr[13]; // 13 bc that's where the BC is stored in extent

    // copy RC
    d->RC = dir_addr[15]; // +2 bc RC is 2 bytes

    // for loop to copy block addresses
    int block_index; // index for the block
    uint8_t block; // place to store block value
    for (block_index = 0; block_index < 16; block_index++){
        block = dir_addr[block_index+16]; // store the block
        // check if the block is valid
        if (block != 0){
            // printf("%d\n", block); // print the block value
            d->blocks[block_index] = block; // copy block data
        }
        else{
            break;
        }
    }

    return d;
}

void writeDirStruct(DirStructType *d, uint8_t index, uint8_t *e)
{
    /* Add your code here */
}

void makeFreeList()
{
    // set all blocks to be free initially
    int i;
    for(i=1; i<NUM_BLOCKS; i++){
        freeList[i] = true; // by default, mark it as unused (true)
    }
    freeList[0] =  false; // the superblock is always in use

    int extent_index; // store the extent number
    int block_index; // store the extent index

    uint8_t *e = (uint8_t*) malloc(BLOCK_SIZE); // define where block 0 data is stored
    blockRead(e, 0); // read data from block 0 into "block0"

    // get the block pointer values from the extent
    for(extent_index = 0; extent_index < 32; extent_index++){
        DirStructType *d = (DirStructType *)malloc(sizeof(DirStructType)); // set value to null
        d = mkDirStruct(extent_index, e); // get the extent data and store it
        if(d->status != 0xe5){
            for(block_index=0; block_index < 16; block_index++){
                // if the block is not invalid
                if(d->blocks[block_index] != 0){
                    freeList[d->blocks[block_index]] = false; // mark as used
                }
            }
        }

        free(d); // free memory for d
    }
    
    free(e); // free memory for e
}

void printFreeList()
{
    int block_row_index; // index of the row for the 16x16 FreeList
    int block_column_index; // index of the column for the 16x16 FreeList

    // for each row (of 16)
    for(block_row_index=0; block_row_index<16; block_row_index++){
        
        // print the hex address of the first block in that row
        printf("%02X: ", block_row_index*16);

        // for each row (of 16)
        for(block_column_index=0; block_column_index<16; block_column_index++){

            // if the block is used
            if(freeList[(block_row_index*16)+block_column_index] == false){
                printf("* ");
            }
            // else
            else{
                printf(". ");
            }
        }
        printf("\n"); // new line for the next row
    }
}

void cpmDir(){
    printf("DIRECTORY LISTING\n");
    uint8_t *e = (uint8_t*) malloc(BLOCK_SIZE); // define where block 0 data is stored
    blockRead(e, 0); // read data from block 0 into "block0"

    int extent_index; // store the extent number
    int block_index; // store the extent index
    
    // int NB; // store the number of fully loaded blocks

    // iterate over extents in block 0
    for(extent_index = 0; extent_index < 32; extent_index++){
        DirStructType *d = (DirStructType *)malloc(sizeof(DirStructType)); // set value to null
        d = mkDirStruct(extent_index, e); // get the extent data and store it
        
        int file_size = 0; // store size of the file in bytes
        int NB = 0; // set the num bytes initially to 0 used
        
        // check if the file is valid
        if(d->status != 0xe5){
            // iterate over blocks in the DirStructType (there are 16)
            for(block_index=0; block_index < 16; block_index++){
                // check if the block is valid
                if(d->blocks[block_index] != 0){
                    NB++;// increase the num bytes used
                }
            }
            NB--; // decrease by 1 to account for partially filled sector
            file_size = (NB * 1024) + (d->RC * 128) + d->BC; // calc file size
            printf("%s.%s %d\n",d->name, d->extension, file_size);// print file name and size
        }
        free(d); // free data used by malloc
    }
    free(e);
}

bool checkLegalName(char *name)
{
    int name_length = strlen(name);
    int name_index;
    int dot_quantity = 0; // stores the number of "." in the file name
    int file_name_size = 0; // track how long the file name is
    int file_ext_size = 0; // track how long the file name is
    char c; // track current char

    for(name_index = 0; name_index < name_length; name_index++){
        c = name[name_index]; // store the current char



        // check that its a char or a number
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            // is a valid char. Check that file anme is right size
            if(dot_quantity == 0){
                file_name_size++;
                if(file_name_size > 8){
                    printf("File Name Error: The file name must be max 8 chars\n");
                    return false;
                }
            }
            else{
                file_ext_size++;
                if(file_ext_size>3){
                    printf("File Name Error: The file ext must be max 3 chars\n");
                    return false;
                }
            }
        }

        // check for '.'
        else if( c == '.'){
            dot_quantity++;
            if(dot_quantity > 1){
                printf("File Name Error: Only 1 '.' is allowed in the file name\n");
                return false;
            }
        }

        // else not a valid char
        else{
            printf("File Name Error: File contains illegal char '%c'\n", c);
            return false;
        }
    }

    if(dot_quantity == 0){
        printf("warning: file name '%s' contains no file extension...\n", name);
    }
    return true; // is a valid file name
}

int findExtentWithName(char *name, uint8_t *block0)
{
    
    // split the file name
    char *dot_ptr = strrchr(name, '.'); // find where the "." is
    char  search_name[9]; // store file name
    char  search_ext[4]; // store file extension

    // copy name
    strncpy(search_name, name, dot_ptr - name); // copy char before "."
    search_name[dot_ptr - name] = '\0'; // null terminate

    // copy extension
    strncpy(search_ext, dot_ptr + 1, strlen(dot_ptr + 1)); // copy char after "."
    search_ext[strlen(dot_ptr + 1)] = '\0'; // null terminate
    
    // iterate over each extent and check file name
    int extent_index; // store the extent number
    for(extent_index = 0; extent_index < 32; extent_index++){
        DirStructType *d = (DirStructType *)malloc(sizeof(DirStructType)); // make room for extent data
        d = mkDirStruct(extent_index, block0); // get the extent data and store it
        if(d->status != 0xe5){

            // check that the files match
            int name_match = strcmp(search_name, d->name);
            int ext_match = strcmp(search_ext, d->extension);
            if(name_match == 0 && ext_match == 0){
                return extent_index;
            }
        }
        free(d); // free data used by malloc
    }

    return -1; // file just not found in directory block
}

int cpmDelete(char *fileName)
{
    uint8_t *e = (uint8_t*) malloc(BLOCK_SIZE); // define where block 0 data is stored
    blockRead(e, 0); // read data from block 0 into "block0"

    int extent_index = findExtentWithName(fileName, e); // find correct extent
    DirStructType *d = (DirStructType *)malloc(sizeof(DirStructType)); // set value to null
    d = mkDirStruct(extent_index, e); // get the extent data and store it
    d->status = 0xe5; // mark as unused

    // remove blocks from free list
    int block_index;
    for(block_index=0; block_index<BLOCKS_PER_EXTENT; block_index++){
        if(d->blocks[block_index] != 0){
            freeList[d->blocks[block_index]] = true; // mark as unused
        }
    }

    // write back to the specified index of the extent in block of memory
    writeDirStruct(d, extent_index, e);

    free(e); // free data from malloc
    free(d); // free data from malloc
}

int cpmRename(char *oldName, char *newName)
{
    bool legal_name = checkLegalName(newName);

    if(legal_name == false){
        return -1;
    }

    // get extent from name
    uint8_t *e = (uint8_t*) malloc(BLOCK_SIZE);
    blockRead(e, 0); // read data from block 0
    int extent_index = findExtentWithName(oldName, e); // find correct extent

    // get extent dir struct
    DirStructType *d = (DirStructType *)malloc(sizeof(DirStructType)); // set value to null
    d = mkDirStruct(extent_index, e); // get the extent data and store it

    // split the file name
    char *dot_ptr = strrchr(newName, '.'); // find where the "." is
    char  name[9]; // store file name
    char  ext[4]; // store file extension

    // copy name
    strncpy(name, newName, dot_ptr - newName); // copy char before "."
    name[dot_ptr - newName] = '\0'; // null terminate

    // copy extension
    strncpy(ext, dot_ptr + 1, strlen(dot_ptr + 1)); // copy char after "."
    ext[strlen(dot_ptr + 1)] = '\0'; // null terminate
    
    // change the name/extension
    strcpy(d->name, name);
    strcpy(d->extension, ext);


    // write back to the specified index of the extent in block of memory
    writeDirStruct(d, extent_index, e);

    free(e); // free data from malloc
    free(d); // free data from malloc
    
}
