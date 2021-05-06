////////////////////////////////////////////////////////////////////////////////
// /___  ___/ / / / _ \    / / \ \  / /     /  ____/ / _____/                 //
//    / /    / / / / \ \  / /   \ \/ /     /  /__   / /____                   //
//   / /    / / / /   \ \/ /    /   /     / ____/  /____  /                   //
//  / /    / / / /     \  /    /   /     / /      /      /  Tiny File system  //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Overview                                                                   //
//  __________________________________                                        //
//  |   |   |   |        |           |                                        //
//  | S | i | d | inodes | user data |                                        //
//  |___|___|___|________|___________|                                        //
//                                                                            //
//  S - superblock file system info                                           //
//  i - inodes bitmap                                                         //
//  d - data bitmap                                                           //
//                                                                            //
//  Consider the disk divided in blocks of sieze 4k.                          //
//  First block has file system info.                                         //
//  i and d are bitmaps to track rispectively inodes and data blocks          //
//  allocation. The remaining blocks contain user data.                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define BLOCK_SIZE 4000
// Size in blocks
#define DISK_SIZE 1000


typedef enum
{
  INDIRECT = 1, // Indirect inode points to a block that containse inodes
                // for the file.
  DIRECTORY = 2 // The inode points to a directory.
} TFS_INODE_FLAGS;

typedef struct
{
  unsigned int id;        // Unique id for the inode
  unsigned int parent_id; // Id of the parent inode, the parent can be a
                          // directory or an inode in case of indirection
  unsigned int filesize;  // Size of the file in bytes
  unsigned int fileblock; // Points to the data block
  TFS_INODE_FLAGS flags;
} INode;

/*
  char const *tfs_lookup_error(
    [in] int error);

  Remarks:
    Lookup function for errors, returns some info.

  Params:
    error => error code to lookup.

  Return:
    On Success => info about the error code.
    On Fail    => "Error code not found." 
*/
char const *tfs_lookup_error(
    int error);

/*
  int tfs_format_disk(
    [in] int block_count)

  Remarks:
    Format the disk need the number of blocks and the size of the INode data.
    I could not decide for what to put in the inode, so use a buffer and put
    whathever you like in there.

  Params:
    block_count  => number of blocks in the disk, each block is BLOCK_SIZE 
                    size.

  Return:
    On Success => returns 0.
    On Fail    => returns not 0, call tfs_lookup_error for more info.
*/
int tfs_format_disk(
    int block_count);

/*
  int tfs_create_file(
    [in] char *file_name,
    [in] int name_len,
    [in] int is_dir);

  Remarks:
    Creates a new file in the disk and add the data to the payload.
    If the length of the data is greather than the length provided in to the 
    format function an error is returned.

  Params:
    file_name   => Pointer to the buffer with the full path name of the new 
                   file.
    name_len    => Length of the file_name.
    is_dir      => Create a directory this is not 0.

  Return:
    On Success => returns 0.
    On Fail    => returns not 0, call tfs_lookup_error for more info.
*/
int tfs_create_file(
    char *file_name,
    int name_len,
    int is_dir);

/*
  int tfs_remove_file(
    [in] char *file_name,
    [in] int name_len,
    [in] int is_recursive);

  Remarks:
    Removes a file, recursive must be not 0 in order to remove not empty 
    folders.

  Params:
    file_name    => Pointer to the buffer with the full path name of the new 
                   file.
    name_len     => Length of the file_name.
    is_recursive => If not 0 removes empty folders.
 
  Return:
    On Success => returns 0.
    On Fail    => returns not 0, call tfs_lookup_error for more info.
*/
int tfs_remove_file(
    char *file_name,
    int name_len,
    int is_recursive);

/*
  unsigned int tfs_open_file(
    [in] char *file_name,
    [in] int name_len,
    [out] int *error);

  Remarks: 
    If the file is successfully open it returns the id of the first inode
    from the file.

  Params:
    file_name => Pointer to the buffer with the full path name of the new 
                 file.
    name_len  => Length of the file_name.
    error     => pointer to an int to return the error code.

  Return:
    On Success => returns the inode id for the file.
    On Fail    => returns 0, call tfs_lookup_error with the value returned 
                  from error for more info.
*/
unsigned int tfs_open_file(
    char *file_name,
    int name_len,
    int *error);

/*
  unsigned int tfs_get_file_size(
    [in] unsigned int inode_id,
    [out] int *error);

  Remarks:
    Returns the size of the file in bytes. 
  
  Param:
    inode_id  => Id of an inode of the file.
    error     => pointer to an int to return the error code.

  Return:
    On Success => returns the size of the file in bytes.
    On Fail    => returns 0, call tfs_lookup_error with the value returned 
                  from error for more info.
*/
unsigned int tfs_get_file_size(
    unsigned int inode_id,
    int *error);

/*
  int tfs_read(
    [in] unsigned int inode_id,
    [in] unsigned int from,
    [out] char *buffer,
    [in] int buffer_len);

  Remarks:
    Reads the file pointed by the inode starting from the 'from' byte to the
    'from + buffer_len' byte.
    If the file finishes before 'from + buffer_len' do not despair the function
    returns the number of bytes read.
  
  Param:
    inode_id    => Id of an inode of the file.
    from        => Start byte to read in the file.
    buffer      => Buffer for the operation result.
    buffer_len  => Size of the buffer for the operation result.

  Return:
    On Success => returns the number of read bytes.
    On Fail    => returns a negative number, call tfs_lookup_error with the
                  with the returned value for more info.
*/
int tfs_read(
    unsigned int inode_id,
    unsigned int from,
    char *buffer,
    int buffer_len);

/*
  int tfs_write(
    [in] unsigned int inode_id,
    [in] unsigned int from,
    [out] char *buffer,
    [in] int buffer_len);

  Remarks:
    Writes the file pointed by the inode starting from the 'from' byte to the
    'from + buffer_len' byte.
    buffer_len bites are always written unless the funcion fails.
    If the write go past the end of the block everything will be handled by the
    tiny fs and a new inode and indirection layer will be added if necessary.
  
  Param:
    inode_id    => Id of an inode of the file.
    from        => Start byte to read in the file.
    buffer      => Buffer for the operation result.
    buffer_len  => Size of the buffer for the operation result.

  Return:
    On Success => returns the number of written bytes.
    On Fail    => returns a negative number, call tfs_lookup_error with the
                  with the returned value for more info.
*/
int tfs_write(
    unsigned int inode_id,
    unsigned int from,
    char *buffer,
    char *buffer_len);