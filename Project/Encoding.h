#pragma once

#include <assert.h>
#include <stdio.h>
#include <string.h>


int base16_encode_size(const char* src);

void base16_encode(char* dst, const unsigned char* src, size_t src_length);

int base16_decode_size(const char* src, size_t src_length);

int base16_decode(char* dst, const char* src, size_t src_length);

int base64_encode_size(size_t length);

void base64_encode(char* dst, const unsigned char* src, size_t src_length);


int base64_decode_size(const char* code, int len);

//int base64_decode(char * text, unsigned char * dst, int numBytes)
int base64_decode(char* dst, const char* src, int src_length);

int base64_encode_size(int len);

int base64_decode_size(int len);
