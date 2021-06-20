#pragma once

#include <array>

// Empty root
std::array<char, 9> const EMPTY_ROOT = {
    '/',  '\0',       // Directory name
    '\0',             // Parent dir name
    0,    0,    0, 0, // Parent inode
    '\0', '\0'        // Double null
};

// Empty Mars
std::array<char, 13> const EMPTY_MARS = {
    'M',  'a',  'r', 's', '\0', // Directory name
    '/',  '\0',                 // Parent dir name
    0,    0,    0,   0,         // Parent dir inode
    '\0', '\0'                  // Double null
};

// Mars
std::array<char, 46> const MARS = {
    'M',  'a',  'r', 's', '\0',                   // Directory name
    '/',  '\0',                                   // Parent dir name
    0,    0,    0,   0,                           // Parent dir name
    'S',  't',  'e', 'r', 'z',  'o',  '\0',       // File name
    1,    0,    0,   0,                           // File inode
    'P',  'i',  's', 't', 'o',  'n',  'e',  '\0', // File name
    2,    0,    0,   0,                           // File inode
    'T',  'u',  'r', 'b', 'o',  '\0',             // File name
    3,    0,    0,   0,                           // File inode
    '\0', '\0'                                    // Double null
};
