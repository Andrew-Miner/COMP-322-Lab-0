// Author: Andrew Miner
// Date: 2/8/2020
// Class: Comp 322
#include <stdio.h>
#include<fcntl.h>
#include <math.h>
#include <string.h>

typedef enum { true, false } bool;

bool isNumber(const char* str);
void printBinary(char binary[9]);
int getDecVal(const char* binChars, int count);
bool isEvenParity(const char* binChars, int count);
int getBinary(int fileDesc, char* buffer, int bufSize);

bool outputBinFile(const char* fileName);
bool outputBinArgs(const int argc, const char* argv[]);

const char* mnemonicASCII[] = { "NUL", "SOH", "STX", "ETX", "EOT", "ENQ",
							   "ACK", "BEL", "BS", "HT", "LF", "VT", "FF",
							   "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3",
							   "DC4", "NAK", "SYN", "ETB", "CAN", "EM",
							   "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE" };

int main(int argc, char* argv[])
{
	if (argc > 1) {
		int i = 1;
		int fileNamePos = -1;
		for (; i < argc; i++) {
			if (argv[i][0] == '-') {
				if(strlen(argv[i]) == 1)
					continue;
				printf("Error: Invalid Input!");
				break;
			}

			if (isNumber(argv[i]) == false) {
				fileNamePos = i;
				break;
			}
		}

		if (i == argc)
			outputBinArgs(argc, argv);
		else if (fileNamePos != -1)
			outputBinFile(argv[i]);
	}

	return 0;
}

void printBinary(char binary[9])
{
	int dec = getDecVal(binary, 8);
	char* parity = (isEvenParity(binary, 8)) ? "EVEN" : "ODD";

	if (dec < 33)
		printf("%8s %8s %8i %8s\n", binary, mnemonicASCII[dec], dec, parity);
	else
		printf("%8s %8c %8i %8s\n", binary, (char)dec, dec, parity);
}

bool outputBinArgs(const int argc, const char* argv[])
{
	printf("%8s %8s %8s %8s\n", "Original", "ASCII", "Decimal", "Parity");
	printf("-------- -------- -------- --------\n");

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-')
			continue;

		char bin[9];
		bin[8] = '\0';

		int j = 0, len = strlen(argv[i]);
		for (; j < len; j++)
			bin[j] = argv[i][j];
		while (j < 8)
			bin[j++] = '0';

		printBinary(bin);
	}
}

bool outputBinFile(const char* fileName)
{
	int fileDesc = open(fileName, O_RDONLY, 0);

	if (fileDesc == -1) {
		printf("Error: The file ");
		printf(fileName);
		printf(" cannot be opened!\n");
		return false;
	}

	printf("%8s %8s %8s %8s\n", "Original", "ASCII", "Decimal", "Parity");
	printf("-------- -------- -------- --------\n");

	char rByte[9];
	rByte[8] = '\0';
	int rVal = 0;
	do {
		rVal = getBinary(fileDesc, rByte, 8);
		printBinary(rByte);
	} while (rVal == 8);

	if (rVal == -1) {
		printf("Warning: read() function returned an error!");
		return false;
	}
	return true;
}

int getBinary(int fileDesc, char* buffer, int bufSize)
{
	int i = 0;
	int rVal = -1;
	while (i < bufSize) {
		char c;
		rVal = read(fileDesc, &c, 1);

		if (rVal == -1 || rVal == 0)
			break;

		if (c == '\n' || c == ' ')
			continue;

		buffer[i++] = c;
	}

	if (i == bufSize)
		return bufSize;

	while (i < bufSize)
		buffer[i++] = '0';

	return rVal;
}

bool isNumber(const char* str)
{
	for (int i = 0; str[i] != '\0'; i++) {
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

bool isEvenParity(const char* binChars, int count)
{
	bool even = false;
	for (int i = 0; i < count; i++)
		if (binChars[i] == '1')
			even = !even;
	return even;
}

int getDecVal(const char* binChars, int count)
{
	int dec = 0;
    for (int i = 1; i < count; i++) { // starts at 1 to skip extra 8th bit
		if (binChars[i] == '0') continue;
		dec += pow(2, count - i - 1);
	}
	return dec;
}
