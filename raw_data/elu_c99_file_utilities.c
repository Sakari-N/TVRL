// Sakari N (C) 2021 No rights reserved. If you are reading this you are big dummy.

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "elu_c99_file_utilities.h"

int elu_c99_get_file_size(FILE* file_handle, size_t* file_size)
{
	size_t current_position = (size_t)ftell(file_handle);
	if (current_position == (size_t)-1)
		return errno;
	if (fseek(file_handle, 0, SEEK_END))
		return errno;
	size_t size = (size_t)ftell(file_handle);
	if (size == (size_t)-1)
	{
		fseek(file_handle, (long)current_position, SEEK_SET);
		return errno;
	}
	if (fseek(file_handle, (long)current_position, SEEK_SET))
		return errno;
	*file_size = size;
	return 0;
}

int elu_c99_load_file(const char* file_name, size_t buffer_size, size_t* file_size, void* file_data)
{
	FILE* handle = fopen(file_name, "rb");
	if (!handle)
		return errno;
	if (fseek(handle, 0, SEEK_END))
	{
		int fseek_error = errno;
		fclose(handle);
		return fseek_error;
	}
	size_t size = (size_t)ftell(handle);
	if (size == (size_t)-1)
	{
		int ftell_error = errno;
		fclose(handle);
		return ftell_error;
	}
	if (fseek(handle, 0, SEEK_SET))
	{
		fclose(handle);
		return EIO;
	}
	*file_size = size;
	if (buffer_size < size)
	{
		fclose(handle);
		return ENOBUFS;
	}
	for (size_t loaded = 0; loaded != size;)
	{
		size_t io_result = fread((void*)((uintptr_t)file_data + loaded), 1, size - loaded, handle);
		if (io_result)
			loaded += io_result;
		else
		{
			fclose(handle);
			return EIO;
		}
	}
	fclose(handle);
	return 0;
}

int elu_c99_store_file(const char* file_name, size_t file_size, const void* file_data)
{
	FILE* handle = fopen(file_name, "wb");
	if (!handle)
		return errno;
	for (size_t stored = 0; stored != file_size;)
	{
		size_t io_result = fwrite((const void*)((uintptr_t)file_data + stored), 1, file_size - stored, handle);
		if (io_result)
			stored += io_result;
		else
		{
			fclose(handle);
			return EIO;
		}
	}
	fclose(handle);
	return 0;
}

int elu_c99_load_and_allocate_file(const char* file_name, size_t* file_size, void** file_data)
{
	FILE* handle = fopen(file_name, "rb");
	if (!handle)
		return errno;
	if (fseek(handle, 0, SEEK_END))
	{
		int fseek_error = errno;
		fclose(handle);
		return fseek_error;
	}
	size_t size = (size_t)ftell(handle);
	if (size == (size_t)-1)
	{
		int ftell_error = errno;
		fclose(handle);
		return ftell_error;
	}
	if (fseek(handle, 0, SEEK_SET))
	{
		fclose(handle);
		return EIO;
	}
	uintptr_t data = (uintptr_t)malloc(size);
	if (!data)
	{
		fclose(handle);
		return ENOMEM;
	}
	for (size_t loaded = 0; loaded != size;)
	{
		size_t io_result = fread((void*)(data + loaded), 1, size - loaded, handle);
		if (io_result)
			loaded += io_result;
		else
		{
			free((void*)data);
			fclose(handle);
			return EIO;
		}
	}
	fclose(handle);
	*file_size = size;
	*file_data = (void*)data;
	return 0;
}

#ifdef __cplusplus
}
#endif // __cplusplus
