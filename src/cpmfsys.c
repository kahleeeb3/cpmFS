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
    char letter; // temp storage for the letter at that index
    for(name_index = 0; name_index<8 ; name_index++){
        letter = dir_addr[name_index+1]; // store the letter found
        // check if the char is a space character (32 in ASCII)
        if( letter != 32){
            // printf("%c\n",letter); // print the letter
            d->name[name_index] = letter; // store the letter
        }
        else{
            break;
        }
    }
    d->name[name_index+1] = '\0'; // add null terminator
    // printf("%s\n", d->name); // print the stored name

    // copy extension + null terminator
    int ext_index; // index for ext
    for(ext_index=0; ext_index<3; ext_index++){
        letter = dir_addr[ext_index+9]; // store the letter found
        // check if the char is a space character (32 in ASCII)
        if( letter != 32){
            // printf("%c\n", letter); // print the letter
            d->extension[ext_index] = letter;
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
    /* Add your code here */
}

void printFreeList()
{
    /* Add your code here */
}

// print all directory entries, just the names and sizes
void cpmDir(){
    printf("DIRECTORY LISTING\n");
    uint8_t block0[BLOCK_SIZE]; // define where block 0 data is stored
    blockRead(block0, 0); // read data from block 0 into "block0"

    int extent_index; // store the extent number
    int block_index; // store the extent index
    int file_size; // store size of the file in bytes
    int NB; // store the number of fully loaded blocks
    DirStructType *cpm_extent = (DirStructType *)malloc(sizeof(DirStructType)); // place to store extent data

    // iterate over extents in block 0
    for(extent_index = 0; extent_index < 32; extent_index++){
        cpm_extent = mkDirStruct(extent_index, block0); // get the extent data and store it
        // check if the file is valid
        if(cpm_extent->status != 0xe5){
            NB = 0; // set the num bytes initially to 0 used
            // iterate over blocks in the DirStructType (there are 16)
            for(block_index=0; block_index < 16; block_index++){
                // check if the block is valid
                if(cpm_extent->blocks[block_index] != 0){
                    NB++;// increase the num bytes used
                }
            }
            NB--; // decrease by 1 to account for partially filled sector
            file_size = NB * 1024 + cpm_extent->RC * 128 + cpm_extent->BC; // calc file size
            printf("%s.%s %d\n",cpm_extent->name, cpm_extent->extension, file_size);// print file name and size
        }
    }
}

bool checkLegalName(char *name)
{
    /* Add your code here */
}

int findExtentWithName(char *name, uint8_t *block0)
{
    /* Add your code here */

    return -1; // file just not found in directory block
}

int cpmDelete(char *fileName)
{
    /* Add your code here */
}

int cpmRename(char *oldName, char *newName)
{
    /* Add your code here */
}
