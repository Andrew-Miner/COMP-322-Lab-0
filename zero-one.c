// Author: Andrew Miner
// Date: 2/8/2020
// Class: Comp 322
#include <stdio.h>
#include<fcntl.h>
#include <math.h>

typedef enum { true, false } bool;

int getBinary(int fileDesc, char* buffer, int bufSize);
bool isEvenParity(const char* binChars, int count);
int getDecVal(const char* binChars, int count);
bool outputBinary(const char* fileName);

const char* mnemonicASCII[] = { "NUL", "SOH", "STX", "ETX", "EOT", "ENQ",
							   "ACK", "BEL", "BS", "HT", "LF", "VT", "FF",
							   "CR", "SO", "SI", "DLE", "DC1", "DC2", "DC3",
							   "DC4", "NAK", "SYN", "ETB", "CAN", "EM",
							   "SUB", "ESC", "FS", "GS", "RS", "US", "SPACE" };

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		int i = 1;
		for (; i < argc; i++)
		{
			if (argv[i][0] == '-')
				continue;
			break;
		}

		if (i < argc)
		{
			outputBinary(argv[i]);
			return 0;
		}
	}

	char fileName[128];
	printf("Enter file: ");
	scanf("%s", fileName);

	outputBinary(fileName);

	return 0;
}

bool outputBinary(const char* fileName)
{
	int fileDesc = open(fileName, O_RDONLY, 0);

	if (fileDesc == -1)
	{
		printf("Error: The file cannot be opened!\n");
		return false;
	}

	printf("%8s %8s %8s %8s\n", "Original", "ASCII", "Decimal", "Parity");
	printf("-------- -------- -------- --------\n");

	char rByte[9];
	rByte[8] = '\0';
	int rVal = 0;
	do
	{
		rVal = getBinary(fileDesc, rByte, 8);

		int dec = getDecVal(rByte, 8);
		char* parity = (isEvenParity(rByte, 8)) ? "EVEN" : "ODD";

		if (dec < 33)
			printf("%8s %8s %8i %8s\n", rByte, mnemonicASCII[dec], dec, parity);
		else
			printf("%8s %8c %8i %8s\n", rByte, (char)dec, dec, parity);

	} while (rVal == 8);

	if (rVal == -1)
	{
		printf("Warning: read() function returned an error!");
		return false;
	}
	return true;
}

int getBinary(int fileDesc, char* buffer, int bufSize)
{
	int i = 0;
	int rVal = -1;
	while (i < bufSize)
	{
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

bool isEvenParity(const char* binChars, int count)
{
	bool even = true;
	for (int i = 0; i < count; i++)
		if (binChars[i] == '1')
			even = !even;
	return even;
}

int getDecVal(const char* binChars, int count)
{
	int dec = 0;
	for (int i = 1; i < count; i++) // starts at 1 to skip extra 8th bit
	{
		if (binChars[i] == '0') continue;
		dec += pow(2, count - i - 1);
	}
	return dec;
}
