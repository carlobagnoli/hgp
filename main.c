#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <openssl/sha.h>
#include <string.h>

void printa(unsigned char* array, int len) {
	for (int i = 0; i < len; i++) {
		printf("%3i, ", array[i]);
	}
	printf("\n");
}

void printx(unsigned char* array, int len) {
	for (int i = 0; i < len; i++) {
		printf("%X", array[i]);
	}
	printf("\n");
}

// Unsigned Char CoPY
void uccpy(unsigned char* a, unsigned char* b, int n, int i) {
	if (i < n) {
		a[i] = b[i];
		uccpy(a, b, n, ++i);
	}
}

typedef struct {
	unsigned char* result;
	int length;
	int remainder;
} Result;

// NOTE: Denominator has to be less than the base
Result division(unsigned char* numerand, int length_of_numerand, int denominator, int base) {
	unsigned char copy_n[length_of_numerand];
	unsigned char* result = malloc(length_of_numerand + 1);

	uccpy(copy_n, numerand, length_of_numerand, 0);

	int i = 0;
	int a[2] = {0, 0};
	int r[2] = {0, 0};
	while (i < length_of_numerand) {
		if (i == 0) {
			a[0] = 0;
			a[1] = copy_n[i];
		} else {
			a[0] = copy_n[i - 1];
			a[1] = copy_n[i];
		}

		result[i] = (a[0] * base + a[1]) / denominator;

		r[0] = (result[i] * denominator) / base;
		r[1] = (result[i] * denominator) % base;

		if (i != 0) {
			copy_n[i - 1] = a[0] - r[0];
		}
		copy_n[i] = a[1] - r[1];

		i++;
	}

	Result rresult;
	rresult.result = result;
	rresult.length = length_of_numerand;
	rresult.remainder = copy_n[length_of_numerand - 1];

	return rresult;
}

char* decode(unsigned char* hash, int length, char* dict) {
	int expected_size = length * log(256) / log(strlen(dict));

	char* result = malloc(expected_size);
	Result res = division(hash, length, strlen(dict), 0x100);

	for (int i = 0; i < expected_size; i++) {
		result[expected_size - i - 1] = dict[res.remainder];
		res = division(res.result, length, strlen(dict), 0x100);
	}

	free(res.result);

	return result;
}

int main() {
	char place_for_password[1<<10] = "facebook";
	char* password = "something";

	strcat(place_for_password, password);

	unsigned char* hash = SHA512((unsigned char*) place_for_password, strlen((const char*) place_for_password), NULL);

	char* dict = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

	char* pwd = decode(hash, SHA512_DIGEST_LENGTH, dict);

	printf("Your password is: %s\n", pwd);

	free(pwd);
}
