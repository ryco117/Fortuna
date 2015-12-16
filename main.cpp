#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fortuna.h"
#include "base64.h"

#define BLOCKS 2
#define RUNS 100000
#define SEED_SIZE 234							//This was calculated assuming attacker may know to 90% certainty value of each bit in seed,\
												entropy is -Sigma(P(x)*log_2(P(x))), and max entropy in generator state (what we're aiming for)\
												is 256 bits of entropy. This assumption is almost certainly impossible, but with the speed of /dev/urandom,\
												it's not dramatically damaging

void Seed(FortunaPRNG* prng);

using namespace std;

int main(int argc, char** argv)
{
	bool unlimited = false;
	bool format64 = false;
	bool print = false;
	unsigned int blocks = BLOCKS;
	unsigned int runs = RUNS;
	stringstream ss;
	for(unsigned int i = 1; i < argc; i++)						//What arguments were we provided with? How should we handle them
	{
		ss.clear();
		string Arg = string(argv[i]);
		if((Arg == "-r" || Arg == "--runs") && i+1 < argc)
		{
			try
			{
				ss.str(argv[i+1]);
				ss >> runs;
				i++;
			}
			catch(ios_base::failure f)
			{
				cout << "Bad input, exiting\n";
				return -1;
			}
		}
		else if((Arg == "-b" || Arg == "--blocks") && i+1 < argc)
		{
			try
			{
				ss.str(argv[i+1]);
				ss >> blocks;
				i++;
			}
			catch(ios_base::failure f)
			{
				cout << "Bad input, exiting\n";
				return -2;
			}
		}
		else if(Arg == "-f" || Arg == "--format64")
			format64 = true;
		else if(Arg == "-p" || Arg == "--print")
			print = true;
		else if(Arg == "-u" || Arg == "--unlimited")
			unlimited = true;
		else if(Arg == "-h" || Arg == "--help")
		{
			cout << "Written by ryco117\n\
contact: ryco117@gmail.com\n\n\
Args List\n\
---------\n\
-r, --runs\t\tThe number of pseudo random numbers to generate\n\
-b, --blocks\t\tThe size of each pseudo random number in blocks (each block is 128 bits)\n\
-f, --format64\t\tEnable formatting to base64 and adding new line chars\n\
-p, --print\t\tPrint to screen instead of writing to out.txt\n\
-u, --unlimited\t\tDisregard runs value and generate until program receives a kill signal\n\
-h, --help\t\tPrint this menu (but you already knew that :P)\n";
			return 0;
		}
		else			//What the hell were they trying to do?
			cout << "warning: didn't understand " << Arg << endl;
	}

	FortunaPRNG prng;
	Seed(&prng);
	if(!print)
	{
		fstream TestFile("out.txt", ios_base::in | ios_base::out | ios::trunc);
		if(TestFile.is_open())
		{
			unsigned char* RandBlk = new unsigned char[blocks*16];
			char* RandBlk64;
			unsigned int curPrint = 0;
			cout << "0%";
			fflush(stdout);
			for(unsigned int i = 0; (i < runs) || unlimited; i++)
			{
				if((unsigned int)(((unsigned long long)(i+1) * (unsigned long long)100)/(unsigned long long)runs) > curPrint)
				{
					curPrint = (unsigned int)(((unsigned long long)(i+1) * (unsigned long long)100)/(unsigned long long)runs);
					printf("\r%u%s", curPrint, "%");
					fflush(stdout);
				}
				try
				{
					prng.GenerateBlocks(RandBlk, blocks);
				}
				catch (const char* str)
				{
					delete[] RandBlk;
					fwrite(str, 1, strlen(str), stdout);
					return -3;
				}
				if(format64)
				{
					RandBlk64 = Base64Encode((const char*)RandBlk, blocks*16);
					TestFile.write(RandBlk64, strlen(RandBlk64));
					TestFile.write("\n", 1);
					delete[] RandBlk64;
				}
				else
					TestFile.write((const char*)RandBlk, blocks*16);
			}
			delete[] RandBlk;
			TestFile.close();
		}
	}
	else
	{
		unsigned char* RandBlk = new unsigned char[blocks*16];
		char* RandBlk64;
		for(unsigned int i = 0; (i < runs) || unlimited; i++)
		{
			prng.GenerateBlocks(RandBlk, blocks);
			if(format64)
			{
				RandBlk64 = Base64Encode((const char*)RandBlk, blocks*16);
				fwrite(RandBlk64, 1, strlen(RandBlk64), stdout);
				fwrite("\n", 1, 1, stdout);
				delete[] RandBlk64;
			}
			else
				fwrite(RandBlk, 16, blocks, stdout);
		}
		delete[] RandBlk;
	}
	fwrite("\n", 1, 1, stdout);
	return 0;
}

void Seed(FortunaPRNG* prng)
{
	//Properly Seed rand()
	FILE* random;

	unsigned char* seed = new unsigned char[SEED_SIZE];
	#ifdef WINDOWS
		RtlGenRandom(seed, SEED_SIZE);
	#else
		random = fopen ("/dev/urandom", "r");		//Unix provides it, why not use it
		if(random == NULL)
		{
			fprintf(stderr, "Cannot open /dev/urandom!\n");
			delete[] seed;
			return;
		}
		fread(seed, 1, SEED_SIZE, random);
		fclose(random);
	#endif
	prng->Seed(seed, SEED_SIZE);
	memset(seed, 0, SEED_SIZE);
	delete[] seed;
}
