#pragma once

// Empty root
// Directory name             /\0
// Parent dir name            \0
// Double null                \0\0
std::array<char, 9> const EMPTY_ROOT = {'/', '\0', 0,    0,   0,
                                        0,   '\0', '\0', '\0'};

// Empty Mars
// Directory name             Mars\0
// Parent dir name            /\
// Parent dir id              0000\0
// Double null                \0\0
std::array<char, 14> const EMPTY_MARS = {'M',  'a',  'r',  's',  '\0',
                                         '/',  '\0', '\0', '\0', '\0',
                                         '\0', '\0', '\0', '\0'};
