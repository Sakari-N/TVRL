// Sakari N (C) 2021 No rights reserved. If you are reading this you are big dummy.

#define _CRT_SECURE_NO_WARNINGS
#include "elu_c99_file_utilities.h"
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct entry_t
{
	int number;
	int count;
	int equal_copies;
	char* data;
	size_t offset;
	size_t length;
	size_t cat_prefix_length;
	size_t cat_nunber_length;
	size_t cat_postfix_length;
	int dash_in_artist_name;
	int slash_in_artist_name;
	int artist_count;
	char* artists;
	size_t artists_length;
	char* name;
	size_t name_length;
	char* download_info;
	size_t download_info_length;
	char* download_data;
	size_t download_data_length;
} entry_t;

void sort_entry_table(size_t length, entry_t* data)
{
	size_t j = (length - 1) / 2;
	if (data[length - 1].number < data[0].number)
	{
		entry_t t = data[length - 1];
		data[length - 1] = data[0];
		data[0] = t;
	}
	if (data[j].number < data[0].number)
	{
		entry_t t = data[j];
		data[j] = data[0];
		data[0] = t;
	}
	if (data[length - 1].number < data[j].number)
	{
		entry_t t = data[length - 1];
		data[length - 1] = data[j];
		data[j] = t;
	}
	int x = data[j].number;

	j = length;
	for (size_t i = (size_t)~0;;)
	{
		while (data[--j].number > x)
			continue;
		while (data[++i].number < x)
			continue;
		if (i < j)
		{
			entry_t t = data[i];
			data[i] = data[j];
			data[j] = t;
		}
		else
			break;
	}
	++j;

	if (j > 1)
		sort_entry_table(j, data);
	if (length - j > 1)
		sort_entry_table(length - j, data + j);
}

static int is_number(char character)
{
	return character >= '0' && character <= '9';
}

static void output_text(int escape, FILE* output, size_t length, const char* text)
{
	if (escape)
	{
		for (size_t i = 0; i != length; ++i)
			if (text[i] == '\\')
				fprintf(output, "\\\\");
			else if (text[i] == '"')
				fprintf(output, "\\\"");
			else
				fprintf(output, "%c", text[i]);
	}
	else
	{
		for (size_t i = 0; i != length; ++i)
			fprintf(output, "%c", text[i]);
	}
}

static size_t line_count(size_t length, const char* text)
{
	size_t count = 1;
	for (size_t i = 0; i != length; ++i)
		if (text[i] == '\n')
			++count;
	return count;
}

int main(int argc, char** argv)
{
	size_t file_size;
	char* file_data;
	int error = elu_c99_load_and_allocate_file("C:\\Users\\Sakari-N\\Desktop\\tsundere_violence_releases_raw.txt", &file_size, (void**)&file_data);
	assert(!error);

	size_t count = line_count(file_size, file_data) - 1;
	entry_t* entry = (entry_t*)malloc(count * sizeof(entry_t));
	assert(entry);

	for (size_t p = 0, i = 0; i != count; ++i)
	{
		entry[i].count = 1;
		entry[i].offset = p;
		entry[i].data = file_data + p;
		entry[i].length = 0;
		while (entry[i].data[entry[i].length] != '\r')
			entry[i].length++;
		assert(entry[i].length);
		assert(entry[i].data[entry[i].length + 1] == '\n');

		entry[i].cat_prefix_length = 0;
		while (!is_number(entry[i].data[entry[i].cat_prefix_length]))
			entry[i].cat_prefix_length++;
		assert(entry[i].cat_prefix_length > 0 && entry[i].cat_prefix_length < 16);

		entry[i].cat_nunber_length = 0;
		while (is_number(entry[i].data[entry[i].cat_prefix_length + entry[i].cat_nunber_length]))
			entry[i].cat_nunber_length++;
		assert(entry[i].cat_nunber_length);

		entry[i].cat_postfix_length = 0;
		while (entry[i].data[entry[i].cat_prefix_length + entry[i].cat_nunber_length + entry[i].cat_postfix_length] != ' ')
			entry[i].cat_postfix_length++;

		assert(entry[i].cat_prefix_length + entry[i].cat_nunber_length + entry[i].cat_postfix_length < entry[i].length);

		entry[i].number = 0;
		for (size_t j = 0; j != entry[i].cat_nunber_length; ++j)
			entry[i].number = (10 * entry[i].number) + (entry[i].data[entry[i].cat_prefix_length + j] - '0');

		assert(
			entry[i].data[entry[i].cat_prefix_length + entry[i].cat_nunber_length + entry[i].cat_postfix_length + 0] == ' ' &&
			entry[i].data[entry[i].cat_prefix_length + entry[i].cat_nunber_length + entry[i].cat_postfix_length + 1] == '-' &&
			entry[i].data[entry[i].cat_prefix_length + entry[i].cat_nunber_length + entry[i].cat_postfix_length + 2] == ' ');

		entry[i].artists = entry[i].data + entry[i].cat_prefix_length + entry[i].cat_nunber_length + entry[i].cat_postfix_length + 3;
		entry[i].artists_length = 0;
		while (!(
			entry[i].artists[entry[i].artists_length + 0] == ' ' &&
			entry[i].artists[entry[i].artists_length + 1] == '-' &&
			entry[i].artists[entry[i].artists_length + 2] == ' '))
		{
			entry[i].artists_length++;
			assert(entry[i].artists + entry[i].artists_length != entry[i].data + entry[i].length);
		}

		assert(
			entry[i].artists[entry[i].artists_length + 0] == ' ' &&
			entry[i].artists[entry[i].artists_length + 1] == '-' &&
			entry[i].artists[entry[i].artists_length + 2] == ' ');

		entry[i].name = entry[i].artists + entry[i].artists_length + 3;
		entry[i].name_length = 0;
		while (!(
			entry[i].name[entry[i].name_length + 0] == ' ' &&
			entry[i].name[entry[i].name_length + 1] == '(' &&
			entry[i].name[entry[i].name_length + 2] == ' '))
		{
			entry[i].name_length++;
			assert(entry[i].name + entry[i].name_length != entry[i].data + entry[i].length);
		}

		assert(
			entry[i].name[entry[i].name_length + 0] == ' ' &&
			entry[i].name[entry[i].name_length + 1] == '(' &&
			entry[i].name[entry[i].name_length + 2] == ' ');

		entry[i].download_info = entry[i].name + entry[i].name_length + 1;
		entry[i].download_info_length = 0;
		while (entry[i].download_info[entry[i].download_info_length] != '\r')
		{
			entry[i].download_info_length++;
		}
		assert(entry[i].download_info_length >= 4);
		assert(entry[i].download_info + entry[i].download_info_length == entry[i].data + entry[i].length);

		entry[i].download_data = entry[i].download_info + 2;
		entry[i].download_data_length = entry[i].download_info_length - 4;

		entry[i].slash_in_artist_name = 0;
		entry[i].dash_in_artist_name = 0;
		entry[i].artist_count = 1;
		for (size_t j = 0; j != entry[i].artists_length;)
		{
			if (entry[i].artists_length - j >= 3 && entry[i].artists[j + 0] == ' ' && entry[i].artists[j + 1] == '/' && entry[i].artists[j + 2] == ' ')
			{
				entry[i].artist_count++;
				j += 3;
			}
			else
			{
				if (entry[i].artists[j] == '-')
					entry[i].dash_in_artist_name = 1;
				else if (entry[i].artists[j] == '\\' || entry[i].artists[j] == '/')
					entry[i].slash_in_artist_name = 1;
				++j;
			}
		}

		/*
		if (0)
		{
			printf("\"");
			output_text(0, stdout, entry[i].artists_length, entry[i].artists);
			printf("\" ->\n");
			for (size_t c = 0, p = 0; c != entry[i].artist_count; ++c)
			{
				size_t artist_length = 0;
				while ((p + artist_length != entry[i].artists_length) && !(((entry[i].artists_length - p) >= 3) && entry[i].artists[p + artist_length + 0] == ' ' && entry[i].artists[p + artist_length + 1] == '/' && entry[i].artists[p + artist_length + 2] == ' '))
					++artist_length;

				if (c)
					printf(", ");
				printf("\"");
				output_text(0, stdout, artist_length, entry[i].artists + p);
				printf("\"");

				p += artist_length + 3;
			}
			printf("\n");
		}
		*/

		p += entry[i].length + 2;
	}

	sort_entry_table(count, entry);

	int max_cat_number = -1;
	size_t unique_count = 0;
	for (size_t i = 0; i != count;)
	{
		int current_number = entry[i].number;
		if (current_number > max_cat_number)
			max_cat_number = current_number;
		size_t j = 1;
		entry[i].count = j;
		while (i + j != count && current_number == entry[i + j].number)
		{
			for (size_t n = entry[i].count, k = 0; k != n; ++k)
				entry[i + k].count++;
			++j;
			entry[i + j].count = 1;
		}
		
		entry[i].equal_copies = 1;
		for (size_t k = 1; entry[i].equal_copies && k != entry[i].count; ++k)
			entry[i].equal_copies = (entry[i].length == entry[i + k].length) && !memcmp(entry[i].data, entry[i + k].data, entry[i].length);
		for (size_t k = 0; k != entry[i].count; ++k)
			entry[i + k].equal_copies = entry[i].equal_copies;
		
		++unique_count;
		i += j;
	}

	/*
	for (size_t i = 0; i != count; i += entry[i].count)
		if (!entry[i].equal_copies)
		{
			for (size_t j = 0; j != entry[i].count; ++j)
			{
				assert(!entry[i + j].equal_copies);

				output_text(0, stdout, entry[i + j].length, entry[i + j].data);
				fprintf(stdout, "\n");
			}
		}
	*/

	FILE* output = 0;
	output = fopen("C:\\Users\\Sakari-N\\Desktop\\tsundere_violence_releases.json", "w");

	const char* note = "This list has been created from the release list of Tsundere Violence website and extended by Sakari N. When reading text from the file do not forget to properly parse escaped text.";
	fprintf(output, "{\n\t\"note\" : \"%s\",\n\t\"list\" : [", note);

	for (size_t i = 0; i != count; i += entry[i].count)
	{
		for (size_t n = entry[i].equal_copies ? 1 : entry[i].count, j = 0; j != n; ++j)
		{
			fprintf(output,
				"%s\n"
				"\t\t{\n"
				"\t\t\t\"cat_number\" : %i,\n",
				(i ? "," : ""), entry[i + j].number);

			fprintf(output, "\t\t\t\"cat\" : \"");
			output_text(1, output, entry[i + j].cat_prefix_length + entry[i + j].cat_nunber_length + entry[i + j].cat_postfix_length, entry[i + j].data);
			fprintf(output, "\",\n");

			fprintf(output, "\t\t\t\"artists\" : [ ");
			assert(entry[i + j].artist_count);
			for (size_t c = 0, p = 0; c != entry[i + j].artist_count; ++c)
			{
				size_t artist_length = 0;
				while ((p + artist_length != entry[i + j].artists_length) && !(((entry[i + j].artists_length - p) >= 3) && entry[i + j].artists[p + artist_length + 0] == ' ' && entry[i + j].artists[p + artist_length + 1] == '/' && entry[i + j].artists[p + artist_length + 2] == ' '))
					++artist_length;
				if (c)
					fprintf(output, ", ");
				fprintf(output, "\"");
				output_text(1, output, artist_length, entry[i + j].artists + p);
				fprintf(output, "\"");
				p += artist_length + 3;
			}
			//output_text(1, output, entry[i].artists_length, entry[i].artists);
			fprintf(output, " ],\n");

			fprintf(output, "\t\t\t\"name\" : \"");
			output_text(1, output, entry[i + j].name_length, entry[i + j].name);
			fprintf(output, "\",\n");

			fprintf(output, "\t\t\t\"link\" : \"");
			output_text(1, output, entry[i + j].download_data_length, entry[i + j].download_data);
			fprintf(output, "\"\n");

			fprintf(output, "\t\t}");
		}
	}

	fprintf(output, "\n\t]\n}");

	fflush(output);
	fclose(output);
	output = 0;

	output = fopen("C:\\Users\\Sakari-N\\Desktop\\tsundere_violence_releases.txt", "w");

	for (size_t i = 0; i != count; i += entry[i].count)
	{
		for (size_t n = entry[i].equal_copies ? 1 : entry[i].count, j = 0; j != n; ++j)
		{
			output_text(0, output, entry[i + j].length, entry[i + j].data);
			fprintf(output, "\n");
		}
	}

	fflush(output);
	fclose(output);
	output = 0;

	size_t json_size;
	char* json_data;
	error = elu_c99_load_and_allocate_file("C:\\Users\\Sakari-N\\Desktop\\tsundere_violence_releases.json", &json_size, (void**)&json_data);
	assert(!error);

	/*
	size_t json_parsed_data = 0;
	aaaaaaaaaaaa
	assert(json_parsed_data);
	
	void* parsed_json = malloc(json_parsed_data);
	assert(parsed_json);

	int json_parse_completed = 0;
	bbbbbbbbbbbbbbb
	assert(json_parse_completed);

	free(parsed_json);
	*/

	free(json_data);

	return 0;
}