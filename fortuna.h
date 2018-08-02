#ifndef FORTUNA_H
#define FORTUNA_H
#include <string.h>
#include <stdlib.h>
extern "C"
{
	#include <libscrypt.h>
}
#include "AES.h"

class FortunaPRNG
{
public:
	FortunaPRNG();
	~FortunaPRNG();
	void Seed(const uint8_t* seed, unsigned int len);
	void GenerateBlocks(uint8_t* out, unsigned int n);
	void CountInc();
private:
	AES BlkCipher;
	uint8_t Key[32];
	uint8_t Counter[16];
	uint8_t ZeroBlk[16];
};
#endif
