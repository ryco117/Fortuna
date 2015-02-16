#ifndef AES_CPP
#define AES_CPP
#include "AES.h"

extern "C"
{
	bool AESNI();
	void EncryptNix(const char* Text, unsigned int size, const uint8_t* IV, const uint8_t* Key, char* Buffer);
	int DecryptNix(const char* Cipher, unsigned int size, const uint8_t* IV, const uint8_t* Key, char* Buffer);
	void EncryptWin(const char* Text, unsigned int size, const uint8_t* IV, const uint8_t* Key, char* Buffer);
	int DecryptWin(const char* Cipher, unsigned int size, const uint8_t* IV, const uint8_t* Key, char* Buffer);
}

void AES::Encrypt(const char* Msg, unsigned int MsgLen, const uint8_t* IV, const uint8_t* Key, char* CipherText)
{
	if(AESNI())
	{
		#ifdef WINDOWS
			EncryptWin(Msg, MsgLen, IV, Key, CipherText);
		#else
			EncryptNix(Msg, MsgLen, IV, Key, CipherText);
		#endif
		return;
	}
	
	mat4 State = mat4((unsigned char)0);											//4x4 Matrix to go from original to cipher text
	mat4 CipherKey[2] = {mat4(Key), mat4(&Key[16])};									//2 4x4 Matrices to hold parts 1 & 2 of the 256 bit key
	mat4 IVKey = mat4(IV);
	
	mat4* Keys = new mat4[15];														//Will hold all 14 round keys and the initial (at pos 0)
	Keys[0] = CipherKey[0];
	Keys[1] = CipherKey[1];

	for(int i = 2; i < 15; i++)
		Keys[i] = NextRound(Keys, i);
	
	unsigned char Pad = 16 - (MsgLen % 16);
	char PaddedBlock[16];
	memcpy(PaddedBlock, &Msg[MsgLen + Pad - 16], 16 - Pad);
	memset(PaddedBlock + (16 - Pad), Pad, Pad);
	
	for(unsigned int i = 0; (i * 16) <= MsgLen; i++)									//For every 16 chars, fill the state matrix again.
	{
		if((MsgLen + Pad) - (16 * i) > 16)
		{
			for(int col = 0; col < 4; col++)
				for(int row = 0; row < 4; row++)
					State.p[col][row] = (unsigned char)Msg[(i * 16) + (4 * col) + row];	//i * 16 controls which block we're on, the rest moves through the block
		}
		else
		{
			for(int col = 0; col < 4; col++)
				for(int row = 0; row < 4; row++)
					State.p[col][row] = (unsigned char)PaddedBlock[(4 * col) + row];
		}
		
		State.AddRoundKey(IVKey);														//This adds more randomness to strings with repeating blocks
		State.AddRoundKey(Keys[0]);
		for(int j = 1; j < 14; j++)
		{
			State.SubBytes();
			State.ShiftRows();
			State.MixColumns();
			State.AddRoundKey(Keys[j]);
		}
		
		State.SubBytes();
		State.ShiftRows();
		
		State.AddRoundKey(Keys[14]);
		
		for(int col = 0; col < 4; col++)
			for(int row = 0; row < 4; row++)
				CipherText[(i * 16) + (4 * col) + row] = State.p[col][row];
		
		IVKey = State;
	}
	CipherKey[0] = 0;
	CipherKey[1] = 0;
	for(int i = 0; i < 15; i++)
		Keys[i] = 0;
	
	delete[] Keys;
	return;
}

//The same as encrypt but in reverse...
int AES::Decrypt(const char* Cipher, unsigned int CipherLen, const uint8_t* IV, const uint8_t* Key, char* PlainText)
{
	if(AESNI())
	{
		#ifdef WINDOWS
			unsigned int l = DecryptWin(Cipher, CipherLen, IV, Key, PlainText);
		#else
			unsigned int l = DecryptNix(Cipher, CipherLen, IV, Key, PlainText);
		#endif
		return l;
	}
	
	mat4 State = mat4((unsigned char)0);
	mat4 CipherKey[2] = {mat4(Key), mat4(&Key[16])};
	
	mat4 IVKey = mat4(IV);
	mat4 NextIV = mat4(0);
	
	mat4* Keys = new mat4[15];
	Keys[0] = CipherKey[0];
	Keys[1] = CipherKey[1];
	for(int i = 2; i < 15; i++)
		Keys[i] = NextRound(Keys, i);
	
	for(unsigned int i = 0; (i * 16) < CipherLen; i++)
	{
		for(int col = 0; col < 4; col++)
			for(int row = 0; row < 4; row++)
				State.p[col][row] = (unsigned char)Cipher[(i * 16) + (4 * col) + row];
		
		NextIV = State;
		State.AddRoundKey(Keys[14]);
		State.RevShiftRows();
		State.RevSubBytes(); 
		
		for(int j = 13; j > 0; j--) { 
			State.AddRoundKey(Keys[j]);
			State.RevMixColumns();
			State.RevShiftRows();
			State.RevSubBytes(); 
		}
		State.AddRoundKey(Keys[0]);
		State.AddRoundKey(IVKey);
		
		for(int col = 0; col < 4; col++)
			for(int row = 0; row < 4; row++)
				PlainText[(i * 16) + (4 * col) + row] = State.p[col][row];

		IVKey = NextIV;
	}
	CipherKey[0] = 0;
	CipherKey[1] = 0;
	for(int i = 0; i < 15; i++)
		Keys[i] = 0;
	
	int len = CipherLen;
	
	int NBytes = (char)PlainText[len-1];
	bool BadPad = false;
	
	if(NBytes > 16 || NBytes == 0)
		BadPad = true;
	for(int i = 0; i < NBytes && !BadPad; i++)										//Time to do a simple integrity check
	{
		if(PlainText[len-1 - i] != NBytes)
			BadPad = true;
	}
	if(BadPad == true)
	{
		return -1;
	}
	len -= NBytes;
	PlainText[len] = 0;
	
	delete[] Keys;
	return len;
}
#endif