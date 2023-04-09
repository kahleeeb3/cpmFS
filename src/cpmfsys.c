#include "cpmfsys.h"
#include "diskSimulator.h"

bool freeList[NUM_BLOCKS];


DirStructType  *mkDirStruct(int index,uint8_t *e) {
  /* Add your code here */

  return d;
}

void writeDirStruct(DirStructType *d,uint8_t index, uint8_t *e) {
  /* Add your code here */

}

void makeFreeList() {
  /* Add your code here */


}

void printFreeList() {
  /* Add your code here */

}

// print all directory entries, just the names and sizes
void cpmDir() {
  /* Add your code here */
}

bool checkLegalName(char *name) {
  /* Add your code here */


}

int findExtentWithName(char *name, uint8_t *block0) {
  /* Add your code here */

  return -1; // file just not found in directory block
}

int cpmDelete(char *fileName) {
  /* Add your code here */

}

int cpmRename(char *oldName, char *newName) {
  /* Add your code here */
}
