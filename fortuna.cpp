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

void FortunaPRNG::Seed(const uint8_t* seed, unsigned int len)
{
	libscrypt_scrypt(Key, 32, seed, len, 16384, 8, 1, Key, 32);
	CountInc();
	return;
}

void FortunaPRNG::GenerateBlocks(uint8_t* out, unsigned int n)
{
	if(memcmp(Counter, ZeroBlk, 16) == 0)
		throw "Generator Not Initialized!";
	if(n == 0)
		return;
	if(n > 65536)
		throw "Size is too large!";
	
	char TempBlk[32];		//AES will pad automatically, don't need padded end though
	for(unsigned int i = 0; i < n; i++)
	{
		BlkCipher.Encrypt((const char*)Counter, 16, ZeroBlk, Key, TempBlk);
		memcpy(&out[i*16], TempBlk, 16);
		CountInc();
	}
	
	//Generate a new key
	char NewKey[32];
	BlkCipher.Encrypt((const char*)Counter, 16, ZeroBlk, Key, TempBlk);
	memcpy(&NewKey[0], TempBlk, 16);
	CountInc();
	BlkCipher.Encrypt((const char*)Counter, 16, ZeroBlk, Key, TempBlk);
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
		if(Counter[i] == 0 && ++i < 16)	//Continue loop if carry and wasn't last byte
			continue;

		break;
	}
	return;
}
#endif
