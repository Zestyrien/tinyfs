#include "tinyfs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUPER_BLOCK 0
#define INODE_BITMAP_BLOCK 1
#define DATA_BITMAP_BLOCK 2
#define INODES_BLOCK 3
#define INODES_BLOCKS_COUNT 1
#define DATA_BLOCKS 10

#define PRINT_LINE_SIZE 20

// Struct with blocks that need to be loaded in memory and used for most
// of the operations.
typedef struct {
  char super[BLOCK_SIZE];
  char inode_bitmap[BLOCK_SIZE];
  char data_bitmap[BLOCK_SIZE];
  char inodes[BLOCK_SIZE * INODES_BLOCKS_COUNT];
  // TO DO need to keep track of modified blocks so I dont need to write all of
  // theam for every change
} InMemoryBlocks;

char DISK[DISK_SIZE * BLOCK_SIZE];

// Some utility functions
InMemoryBlocks *getInMemoryBlocks() {
  static char initialized = 0;
  static InMemoryBlocks blocks;

  if (initialized == 0) {
    initialized = 1;
    memcpy(blocks.super, &DISK[SUPER_BLOCK], BLOCK_SIZE);
    memcpy(blocks.inode_bitmap, &DISK[INODE_BITMAP_BLOCK * BLOCK_SIZE],
           BLOCK_SIZE);
    memcpy(blocks.data_bitmap, &DISK[DATA_BITMAP_BLOCK * BLOCK_SIZE],
           BLOCK_SIZE);
    memcpy(blocks.inodes, &DISK[INODES_BLOCK * BLOCK_SIZE],
           BLOCK_SIZE * INODES_BLOCKS_COUNT);
  }

  return &blocks;
}

void writeBlock(char *block_value, int block_index) {
  if(block_index != INODES_BLOCK) return; 
  // TO DO out of range write?
  memcpy(&DISK[block_index * BLOCK_SIZE], block_value, BLOCK_SIZE - 1);

  printf("Write block index: %i\n", block_index);

  char asciiBuff[PRINT_LINE_SIZE + 1] = {};

  for (int i = 0; i < BLOCK_SIZE; i++) {
    printf(" %02x", block_value[i] & 0xFF);

    asciiBuff[i % PRINT_LINE_SIZE] =
        isprint(block_value[i]) ? block_value[i] : '.';

    if ((i % PRINT_LINE_SIZE) == PRINT_LINE_SIZE - 1) {
      printf("\t%s\n", asciiBuff);
    }
  }

  printf("\n");
}

void writeInMemoryBlocks() {
  InMemoryBlocks *blocks = getInMemoryBlocks();
  writeBlock(blocks->super, SUPER_BLOCK);
  writeBlock(blocks->inode_bitmap, INODE_BITMAP_BLOCK);
  writeBlock(blocks->data_bitmap, DATA_BITMAP_BLOCK);
  for (int i = 0; i < INODES_BLOCKS_COUNT; i++) {
    writeBlock(blocks->inodes + i * BLOCK_SIZE, INODES_BLOCK + i);
  }
}

char *readBlock(int block_index) {
  // TO DO out of range read?
  char *block = (char *)malloc(BLOCK_SIZE);
  memcpy(block, &DISK[block_index * BLOCK_SIZE], BLOCK_SIZE);
  return block;
}

void setBlockToZero(int block_index) {
  // Block full of zeros
  static char zeroBlock[BLOCK_SIZE] = {0};
  writeBlock(zeroBlock, block_index);
}

int getFirstZeroBit(char *block, int size) {
  // TO DO Naive implementation
  for (int i = 0; i < size; i++) {
    if ((block[i] & 1) == 0) {
      return i * 8;
    } else if ((block[i] & 2) == 0) {
      return i * 8 + 1;
    } else if ((block[i] & 4) == 0) {
      return i * 8 + 2;
    } else if ((block[i] & 8) == 0) {
      return i * 8 + 3;
    } else if ((block[i] & 16) == 0) {
      return i * 8 + 4;
    } else if ((block[i] & 32) == 0) {
      return i * 8 + 5;
    } else if ((block[i] & 64) == 0) {
      return i * 8 + 6;
    } else if ((block[i] & 128) == 0) {
      return i * 8 + 7;
    }
  }

  return -1;
}

void setBit(char *block, int index) {
  int real_index = index / 8;

  block[real_index] = block[real_index] | (1 << (index % 8));
};
// end of utility function

// intenal use only

/*
  INode* tfs_allocate_inode(
    [out] int *error);

  Remarks:
    Marks an inode as in use in the bitmap and returns it.
    The blocks are not modified in case of error.

  Param:
    error => pointer to an int to return the error code.

  Return:
    On Success => returns a pointer to the inode.
    On Fail    => returns 0, call tfs_lookup_error for more info.
*/
INode *tfs_allocate_inode(int *error) {

  INode *new_inode = 0;

  InMemoryBlocks *blocks = getInMemoryBlocks();
  // Get the index in the bitmap for the inode
  int inodeIndex = getFirstZeroBit(blocks->inode_bitmap, BLOCK_SIZE);
  if (inodeIndex == -1 ||
      inodeIndex >= (BLOCK_SIZE * INODES_BLOCKS_COUNT / sizeof(INode))) {
    // TO DO error = OUT_OF_INODES;
    return new_inode;
  }

  // Get the index in the bitmap for the block
  int dataIndex = getFirstZeroBit(blocks->data_bitmap, BLOCK_SIZE);
  if (dataIndex == -1 || dataIndex > (BLOCK_SIZE * 8)) {
    // TO DO error = OUT_OF_DATA_BLOCKS;
    return new_inode;
  }

  // At this point there is space for the new file
  // Mark the bits in the bitmaps
  setBit(blocks->inode_bitmap, inodeIndex);
  setBit(blocks->data_bitmap, dataIndex);

  new_inode = (INode *)&(blocks->inodes[inodeIndex * sizeof(INode)]);

  // Allocate the inode
  new_inode->id = inodeIndex + INODES_BLOCK;
  new_inode->fileblock = dataIndex + DATA_BLOCKS;
  new_inode->filesize = 0;
  new_inode->flags = (TFS_INODE_FLAGS)-1;

  return new_inode;
}

/*
  int tfs_new_directory(
    [out] char *block,
    [in] char *dir_name,
    [in] int name_len,
    [in] int parent_inode_index)

  Remarks:
    Initialize a new directory file.
    Directory files have the following format:
    ////////////////////////////////////
    //  directory_name;               //
    //  inode_for_parent_directory;   //
    //  inode_for_file1;              //
    //  indde_for_file2;              //
    //  ...                           //
    //  ;;;                           //
    ////////////////////////////////////
    directory_name is the name of the current directory.
    inode_for_parent_directory is the inode index of the parent directory.
    The two above are forllowed by the inode indexes of the files contained in
    the directory. The files can be directory.
    At the end of the directory file there is a triple semicolumn.
    All the fields are separated by a ';'. The ';' is not supported as a
    directory or file name.

  Params:
    block               => Pointer to the data block for the file.
    dir_name            => Pointer to the buffer with the directory name.
    name_len            => Length of the directory name.
    parent_inode_index  => Index of the parent directory inode.

  Return:
    On Success => returns the size of the directory 
*/
int tfs_init_directory(char *block, char *dir_name, int name_len,
                        int parent_inode_index) {
  memcpy(block, dir_name, name_len);
  block[name_len] = ';';
  memcpy(&block[name_len + 1], (char *)&parent_inode_index, sizeof(int));
  block[name_len + 1 + sizeof(int)] = ';';

  // File terminator
  block[name_len + 2 + sizeof(int)] = ';';
  block[name_len + 3 + sizeof(int)] = ';';
  block[name_len + 4 + sizeof(int)] = ';';

  // 5 semicolumns
  return 5 + name_len + sizeof(int);
}
// end of internal use only

int tfs_format_disk(int block_count) {
  // TO DO use block_count or remove it

  InMemoryBlocks *blocks = getInMemoryBlocks();
  char zeroBlock[BLOCK_SIZE] = {};

  // TO DO Superblock
  memcpy(blocks->super, zeroBlock, BLOCK_SIZE);
  memcpy(blocks->inode_bitmap, zeroBlock, BLOCK_SIZE);
  memcpy(blocks->data_bitmap, zeroBlock, BLOCK_SIZE);

  int result;
  INode *file_node = tfs_allocate_inode(&result);
  if (file_node == 0) {
    return result;
  }

  // Set directory flag
  file_node->flags = DIRECTORY;
  file_node->parent_id = -1;

  // Need to write the new block with the directory file
  char rootFolderBlock[BLOCK_SIZE];
  file_node->filesize = tfs_init_directory(rootFolderBlock, "//", 2, file_node->parent_id);

  writeBlock(rootFolderBlock, file_node->fileblock);

  // Write changes to in memory blocks on the disk
  writeInMemoryBlocks();
  return 0;
}

int tfs_create_file(char *file_name, int name_len, int is_dir) {
  int result = 0; // It feels like Windows' ERROR_SUCCESS...

  INode *file_node = tfs_allocate_inode(&result);
  if (file_node == 0) {
    return 0;
  }

  // Set directory flag
  if (is_dir > 0) {
    file_node->flags = DIRECTORY;
  }

  // Need to write the file name in the parent directory.
  // INode *parent_dir = tfs_get_parent_directory(file_name, name_len);

  return result;
}
