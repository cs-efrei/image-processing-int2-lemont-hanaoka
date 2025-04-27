#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>

void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);

#endif
