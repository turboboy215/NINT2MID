/*Nintendo (Hirokazu Tanaka) (GB/GBC) to MIDI converter*/
/*By Will Trowbridge*/
/*Portions based on code by ValleyBell*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define bankSize 16384


FILE* rom, * txt;
long bank;
long tablePtrLoc;
long tableOffset;
long songLoc;
int i, j, k, z;
int stepSize;
char outfile[100000];
int songNum;
long songPtrs[4];
long speedPtr;
long speedPtr2;
long bankAmt;
long nextPtr;
int highestSeq;
int endList = 0;
int songTranspose = 0;
int startTrans = 0;
unsigned static char* romData;
unsigned long seqList[700];
unsigned long patList[700];
unsigned long songList[90];
void seqs2txt(unsigned long list[]);

/*Most common variant*/
const unsigned char MagicBytes[3] = { 0xE6, 0x1F, 0xCD };

/*Lunar Chase*/
const unsigned char MagicBytes2[3] = { 0xE6, 0x3F, 0xCD };

/*Game Boy Wars*/
const unsigned char MagicBytes3[5] = { 0xC3, 0x78, 0x77, 0x47, 0x21 };

/*Game Boy Camera*/
const unsigned char MagicBytes4[3] = { 0xE6, 0x7F, 0xCD };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
void song2txt(int songNum, long ptrList[4], long speedPtr);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
}

static void Write8B(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = value;
}

static void WriteBE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF000000) >> 24;
	buffer[0x01] = (value & 0x00FF0000) >> 16;
	buffer[0x02] = (value & 0x0000FF00) >> 8;
	buffer[0x03] = (value & 0x000000FF) >> 0;

	return;
}

static void WriteBE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF0000) >> 16;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0x0000FF) >> 0;

	return;
}

static void WriteBE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0xFF00) >> 8;
	buffer[0x01] = (value & 0x00FF) >> 0;

	return;
}

int main(int args, char* argv[])
{
	printf("Nintendo (Hirokazu Tanaka) (GB/GBC) to TXT converter\n");
	if (args != 3)
	{
		printf("Usage: NINT2TXT <rom> <bank>\n");
		return -1;
	}
	else
	{
		if ((rom = fopen(argv[1], "rb")) == NULL)
		{
			printf("ERROR: Unable to open file %s!\n", argv[1]);
			exit(1);
		}
		else
		{
			if ((rom = fopen(argv[1], "rb")) == NULL)
			{
				printf("ERROR: Unable to open file %s!\n", argv[1]);
				exit(1);
			}
			else
			{
				bank = strtol(argv[2], NULL, 16);
				if (bank != 1)
				{
					bankAmt = bankSize;
				}
				else
				{
					bankAmt = 0;
				}
			}

			if (bank != 1)
			{
				fseek(rom, ((bank - 1) * bankSize), SEEK_SET);
				romData = (unsigned char*)malloc(bankSize);
				fread(romData, 1, bankSize, rom);
				fclose(rom);
			}

			else
			{
				fseek(rom, ((bank - 1) * bankSize * 2), SEEK_SET);
				romData = (unsigned char*)malloc(bankSize * 2);
				fread(romData, 1, bankSize * 2, rom);
				fclose(rom);
			}

			/*Try to search the bank for base table*/
			for (i = 0; i < bankSize; i++)
			{
				if ((!memcmp(&romData[i], MagicBytes, 3) && ReadLE16(&romData[i + 3]) < 0x8000))
				{
					/*Super Mario Land*/
					if (romData[i - 1] == 0x78)
					{
						tablePtrLoc = bankAmt + i - 3;
					}
					/*Tetris/Dr. Mario/etc.*/
					else
					{
						tablePtrLoc = bankAmt + i - 2;
					}

					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					break;
				}
			}

			/*Try to search the bank for base table*/
			for (i = 0; i < bankSize; i++)
			{
				if ((!memcmp(&romData[i], MagicBytes2, 3) && ReadLE16(&romData[i + 3]) < 0x8000))
				{
					/*Super Mario Land*/
					if (romData[i - 1] == 0x78)
					{
						tablePtrLoc = bankAmt + i - 3;
					}
					/*Tetris/Dr. Mario/etc.*/
					else
					{
						tablePtrLoc = bankAmt + i - 2;
					}

					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					break;
				}
			}

			/*Try to search the bank for base table*/
			for (i = 0; i < bankSize; i++)
			{
				if ((!memcmp(&romData[i], MagicBytes3, 5) && ReadLE16(&romData[i + 5]) < 0x8000))
				{
					/*Super Mario Land*/
					if (romData[i - 1] == 0x78)
					{
						tablePtrLoc = bankAmt + i - 3;
					}
					/*Tetris/Dr. Mario/etc.*/
					else
					{
						tablePtrLoc = bankAmt + i + 5;
					}

					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					break;
				}
			}
			/*Try to search the bank for base table*/
			for (i = 0; i < bankSize; i++)
			{
				if ((!memcmp(&romData[i], MagicBytes4, 3) && ReadLE16(&romData[i + 3]) < 0x8000))
				{
					/*Super Mario Land*/
					if (romData[i - 1] == 0x78)
					{
						tablePtrLoc = bankAmt + i - 3;
					}
					/*Tetris/Dr. Mario/etc.*/
					else
					{
						tablePtrLoc = bankAmt + i - 2;
					}

					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					break;
				}
			}


			if (tableOffset != 0)
			{
				for (i = 0; i < 700; i++)
				{
					seqList[i] = 0;
				}
				for (i = 0; i < 700; i++)
				{
					patList[i] = 0;
				}
				for (i = 0; i < 90; i++)
				{
					songList[i] = 0;
				}
				k = 0;
				songNum = 1;
				i = tableOffset;
				highestSeq = 0;
				z = 0;

				while (ReadLE16(&romData[i - bankAmt]) < (bankSize * 2) && endList == 0)
				{

					songLoc = ReadLE16(&romData[i - bankAmt]);

					if (ReadLE16(&romData[songLoc + 1 - bankAmt]) < (bankSize * 2) && ReadLE16(&romData[songLoc + 1 - bankAmt]) > bankAmt)
					{

						if (songNum == 1)
						{
							stepSize = ReadLE16(&romData[songLoc + 1 - bankAmt]);
							startTrans = romData[songLoc - bankAmt];
						}

						/*Fix for Super Mario Land 2*/
						if (stepSize == 0x5C61 && startTrans == 4)
						{
							printf("Song %i: %04X\n", songNum, songLoc);
							songTranspose = (signed char)romData[songLoc - bankAmt] / 2;
							printf("Song %i transpose: %i\n", songNum, songTranspose);
							speedPtr = ReadLE16(&romData[songLoc + 1 - bankAmt]);
							printf("Song %i step table 1: %04X\n", songNum, speedPtr);
							speedPtr2 = ReadLE16(&romData[songLoc + 3 - bankAmt]);
							printf("Song %i step table 2: %04X\n", songNum, speedPtr2);
							songPtrs[0] = ReadLE16(&romData[songLoc + 5 - bankAmt]);
							printf("Song %i channel 1: %04X\n", songNum, songPtrs[0]);
							songPtrs[1] = ReadLE16(&romData[songLoc + 7 - bankAmt]);
							printf("Song %i channel 2: %04X\n", songNum, songPtrs[1]);
							songPtrs[2] = ReadLE16(&romData[songLoc + 9 - bankAmt]);
							printf("Song %i channel 3: %04X\n", songNum, songPtrs[2]);
							songPtrs[3] = ReadLE16(&romData[songLoc + 11 - bankAmt]);
							printf("Song %i channel 4: %04X\n", songNum, songPtrs[3]);

							patList[k] = songPtrs[0];
							patList[k + 1] = songPtrs[1];
							patList[k + 2] = songPtrs[2];
							patList[k + 3] = songPtrs[3];

							songList[z] = songLoc;

							i += 2;
							k += 4;
							z++;
						}

						else
						{
							printf("Song %i: %04X\n", songNum, songLoc);
							songTranspose = (signed char)romData[songLoc - bankAmt] / 2;
							printf("Song %i transpose: %i\n", songNum, songTranspose);
							speedPtr = ReadLE16(&romData[songLoc + 1 - bankAmt]);
							printf("Song %i step table: %04X\n", songNum, speedPtr);
							songPtrs[0] = ReadLE16(&romData[songLoc + 3 - bankAmt]);
							printf("Song %i channel 1: %04X\n", songNum, songPtrs[0]);
							songPtrs[1] = ReadLE16(&romData[songLoc + 5 - bankAmt]);
							printf("Song %i channel 2: %04X\n", songNum, songPtrs[1]);
							songPtrs[2] = ReadLE16(&romData[songLoc + 7 - bankAmt]);
							printf("Song %i channel 3: %04X\n", songNum, songPtrs[2]);
							songPtrs[3] = ReadLE16(&romData[songLoc + 9 - bankAmt]);
							printf("Song %i channel 4: %04X\n", songNum, songPtrs[3]);

							patList[k] = songPtrs[0];
							patList[k + 1] = songPtrs[1];
							patList[k + 2] = songPtrs[2];
							patList[k + 3] = songPtrs[3];

							songList[z] = songLoc;

							i += 2;
							k += 4;
							z++;
						}

						song2txt(songNum, songPtrs, speedPtr);
						songNum++;
					}
					else
					{
						endList = 1;
					}

				}
				seqs2txt(seqList);
			}
			else
			{
				printf("ERROR: Magic bytes not found!\n");
				exit(-1);
			}
			printf("The operation was successfully completed!\n");
		}
	}
}

void song2txt(int songNum, long ptrList[4], long speedPtr)
{
	int patPos = 0;
	int seqPos = 0;
	int curSeq = 0;
	int songLen = 0;
	int length = 0;
	long command[4];
	long lowestPat = 0;
	int curChan = 0;
	int endSeq = 0;
	int endChan = 0;
	int curTempo = 0;
	int curVol = 0;
	int lowestChan = 0;
	sprintf(outfile, "song%d.txt", songNum);
	if ((txt = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file song%d.txt!\n", songNum);
		exit(2);
	}
	else
	{
		/*Find the ending of non-looping songs*/
		curChan = 0;
		for (curChan == 0; curChan < 4; curChan++)
		{
			if (songPtrs[curChan] != 0)
			{
				lowestPat = songPtrs[curChan];
				break;
			}
		}

		if (curChan == 4)
		{
			lowestPat = 0;
		}

		lowestChan = curChan;
		for (curChan = 1; curChan < 4; curChan++)
		{
			if (songPtrs[curChan] < lowestPat && songPtrs[curChan] != 0)
			{
				lowestPat = songPtrs[curChan];
			}
		}

		patPos = lowestPat - bankAmt;
		while (ReadLE16(&romData[patPos]) != 0x0000 && ReadLE16(&romData[patPos]) != 0xFFFF)
		{
			patPos += 2;
			length++;
		}
		if (ReadLE16(&romData[patPos]) == 0x0000)
		{
			songLen = length;
		}

		else if (ReadLE16(&romData[patPos]) == 0xFFFF)
		{
			songLen = 0;
		}

		for (curChan = 0; curChan < 4; curChan++)
		{
			endChan = 0;
			length = 0;
			patPos = songPtrs[curChan] - bankAmt;

			if (songPtrs[curChan] == 0)
			{
				endChan = 1;
			}
			fprintf(txt, "Channel %i:\n", curChan + 1);

			while (endChan == 0)
			{
				curSeq = ReadLE16(&romData[patPos]);
				if (curSeq != 0x0000 && curSeq != 0xFFFF)
				{
					if (length >= songLen - 1)
					{
						if (songLen != 0)
						{
							endChan = 1;
						}
					}
					seqPos = curSeq - bankAmt;
					fprintf(txt, "Sequence position: %04x\n", curSeq);
					for (j = 0; j < 700; j++)
					{
						if (seqList[j] == curSeq)
						{
							break;
						}
					}
					if (j == 700)
					{
						seqList[highestSeq] = curSeq;
						highestSeq++;
					}
					patPos += 2;
					length++;
				}
				else if (curSeq == 0x0000)
				{
					fprintf(txt, "End of song, no loop\n");
					songLen = length;
					endChan = 1;
				}
				else if (curSeq == 0xFFFF)
				{
					fprintf(txt, "End of song, loop: %04X\n", ReadLE16(&romData[patPos + 2]));
					endChan = 1;
				}
			}
		}
		fclose(txt);
	}
}

void seqs2txt(unsigned long list[])
{
	int seqPos = 0;
	int songEnd = 0;
	int lowestSeq = 0;
	int curSeq = 0;

	long command[4];
	unsigned char lowNibble = 0;
	unsigned char highNibble = 0;
	int curChan = 0;
	int endSeq = 0;
	int endChan = 0;
	int transpose = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int curVol = 0;
	int repeat = 0;
	long jumpPos = 0;
	long jumpPosAbs = 0;
	long stepTable = 0;

	sprintf(outfile, "seqs.txt");
	if ((txt = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file seqs.txt!\n");
		exit(2);
	}
	else
	{
		seqPos = seqList[curSeq];

		while (seqList[curSeq] != 0 && endSeq == 0)
		{
			if (seqPos == seqList[curSeq])
			{
				fprintf(txt, "Sequence 0x%04X:\n", seqList[curSeq]);
			}
			command[0] = romData[seqPos - bankAmt];
			command[1] = romData[seqPos + 1 - bankAmt];
			command[2] = romData[seqPos + 2 - bankAmt];
			command[3] = romData[seqPos + 3 - bankAmt];

			for (k = 0; k < 700; k++)
			{
				if (seqPos == patList[k])
				{
					curSeq++;
					seqPos = seqList[curSeq];

				}
			}

			for (z = 0; z < 90; z++)
			{
				if (seqPos == songList[z])
				{
					curSeq++;
					seqPos = seqList[curSeq];
				}
			}

			if (seqPos < 0x4200)
			{
				endSeq = 1;
			}
			/*End of sequence*/
			if (command[0] == 0x00)
			{
				fprintf(txt, "End of sequence\n\n");
				curSeq++;
				seqPos = seqList[curSeq];
			}

			/*Rest (or low note)*/
			else if (command[0] > 0x00 && command[0] < 0x10)
			{
				if (command[0] % 2)
				{
					fprintf(txt, "Rest: Length %i\n", command[0]);
				}
				else
				{
					curNote = command[0];
					fprintf(txt, "Note: %1X\n", curNote);
				}

				seqPos++;
			}

			/*Play note*/
			else if (command[0] >= 0x10 && command[0] <= 0x9A)
			{
				curNote = command[0];
				fprintf(txt, "Note: %1X\n", curNote);
				seqPos++;
			}

			/*Repeat point start*/
			else if (command[0] == 0x9B)
			{
				repeat = command[1];
				fprintf(txt, "Repeat point start: %i times\n", repeat);
				seqPos += 2;
			}

			/*End of repeat point*/
			else if (command[0] == 0x9C)
			{
				fprintf(txt, "End of repeat point\n");
				seqPos++;
			}

			/*Set parameters*/
			else if (command[0] == 0x9D)
			{
				curVol = command[1];
				fprintf(txt, "Set volume: %i, sweep: %i, duty: %i\n", curVol, command[2], command[3]);
				seqPos += 4;
			}

			/*Change step table*/
			else if (command[0] == 0x9E)
			{
				stepTable = ReadLE16(&romData[seqPos - bankAmt + 1]);
				fprintf(txt, "Change step table: %04X\n", stepTable);
				seqPos += 3;
			}

			/*Transpose (all channels)*/
			else if (command[0] == 0x9F)
			{
				transpose = (signed char)command[1] / 2;
				fprintf(txt, "Transpose: %i\n", transpose);
				seqPos += 2;
			}

			/*Set note length*/
			else if (command[0] >= 0xA0)
			{
				curNoteLen = command[0] - 0xA0;
				fprintf(txt, "Set note length: %i\n", curNoteLen);
				seqPos++;
			}
		}
		fclose(txt);
	}
}