#ifndef FORTUNA_CPP
#define FORTUNA_CPP
#include "fortuna.h"

FortunaPRNG::FortunaPRNG()
{
	memset(Key, 0, 32);
	memset(Counter, 0, 16);
	memset(ZeroBlk, 0, 16);
}

FortunaPRNG::~FortunaPRNG()
{
	memset(Key, 0, 32);
	memset(Counter, 0, 16);
	memset(ZeroBlk, 0, 16);
}

void FortunaPRNG::Seed(const unsigned  char* seed, unsigned int len)
{
	libscrypt_scrypt(Key, 32, seed, len, 16384, 8, 1, Key, 32);
	CountInc();
	return;
}

void FortunaPRNG::GenerateBlocks(unsigned char* out, unsigned int n)
{
	if(memcmp(Counter, ZeroBlk, 16) == 0)
		throw "Generator Not Initialized!";
	if(n == 0)
		return;
	if(n > 65536)
		throw "Size is too large!";
	
	char TempBlk[32];		//AES pads automatically, don't need padded end though
	for(unsigned int i = 0; i < n; i++)
	{
		BlkCipher.Encrypt(Counter, 16, ZeroBlk, Key, TempBlk);
		memcpy(&out[i*16], TempBlk, 16);
		CountInc();
	}
	
	//Generate a new key
	char NewKey[32];
	BlkCipher.Encrypt(Counter, 16, ZeroBlk, Key, TempBlk);
	memcpy(&NewKey[0], TempBlk, 16);
	CountInc();
	BlkCipher.Encrypt(Counter, 16, ZeroBlk, Key, TempBlk);
	memcpy(&NewKey[16], TempBlk, 16);
	CountInc();
	memcpy(Key, NewKey, 32);
	
	memset(NewKey, 0, 32);
	memset(TempBlk, 0, 32);
	return;
}

void FortunaPRNG::CountInc()
{
	unsigned char i = 0;
	while(true)
	{
		Counter[i] += 1;
		if(Counter[i] == 0 && i != 15)	//Continue loop if carry and wasn't last byte
			continue;
		else							//Unnecessary else..
			break;
	}
	return;
}
#endif