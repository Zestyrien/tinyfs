#pragma once

#define BLOCK_SIZE 4000

#define SUPER_BLOCK 0
#define INODE_BITMAP_BLOCK 1
#define DATA_BITMAP_BLOCK 2
#define INODES_BLOCK 3
#define INODES_BLOCK_COUNT 5
#define HASH_BLOCK 8
#define HASH_BLOCK_COUNT 5
#define DATA_BLOCKS 14

#include <array>
#include <vector>
#include <memory>
#include <optional>
#include <string_view>
