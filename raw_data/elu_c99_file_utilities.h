// Sakari N (C) 2021 No rights reserved. If you are reading this you are big dummy.

#ifndef ELU_C99_FILE_UTILITIES_H
#define ELU_C99_FILE_UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define _CRT_SECURE_NO_WARNINGS

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int elu_c99_get_file_size(FILE* file_handle, size_t* file_size);

int elu_c99_load_file(const char* file_name, size_t buffer_size, size_t* file_size, void* file_data);

int elu_c99_store_file(const char* file_name, size_t file_size, const void* file_data);

int elu_c99_load_and_allocate_file(const char* file_name, size_t* file_size, void** file_data);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SSN_C_STD_FILE_UTILITIES_H
