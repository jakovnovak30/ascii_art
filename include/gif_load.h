#pragma once

typedef struct gif_result_t {
	int delay;
	unsigned char *data;
	struct gif_result_t *next;
} gif_result;

unsigned char *stbi_xload(char const *filename, int *x, int *y, int *frames);
