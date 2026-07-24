/* Blowfish Encryption Class - Header File */

#ifndef ___BLOWFISH_H___
#define ___BLOWFISH_H___

#define NUM_SUBKEYS   18
#define NUM_S_BOXES   4
#define NUM_ENTRIES   256

#define MAX_STRING   256
#define MAX_PASSWD   56   /* 56 bytes = 448 bits */



/*   Define LITTLE_ENDIAN (Intel, DCBA) or BIG_ENDIAN (Motorola, ABCD) or VAX (BADC).
      If one of these is defined in another file, you can comment out the following line. */
#define LITTLE_ENDIAN


#ifdef BIG_ENDIAN
   struct BFWordByte
   {
      unsigned int zero:8;
      unsigned int one:8;
      unsigned int two:8;
      unsigned int three:8;
   };
#endif /* endif BIG_ENDIAN (Intel, DCBA) defined */

#ifdef LITTLE_ENDIAN
   struct BFWordByte
   {
      unsigned int three:8;
      unsigned int two:8;
      unsigned int one:8;
      unsigned int zero:8;
   };
#endif /* endif LITTLE_ENDIAN (Motorola, ABCD) defined */

#ifdef VAX
   struct BFWordByte
   {
      unsigned int one:8;
      unsigned int zero:8;
      unsigned int three:8;
      unsigned int two:8;
   };
#endif /* endif VAX (BADC) defined */

union BFWord
{
   unsigned int word;
   BFWordByte byte;
};

struct BFDWord
{
   BFWord word0;
   BFWord word1;
};


class CBlowfish
{
public:
   CBlowfish();
   ~CBlowfish();

   void Reset();

   void SetPassword(char *Passwd);

   void Encrypt(void *Ptr, unsigned long nBytes);
   void Decrypt(void *Ptr, unsigned long nBytes);
   uint8_t _crc;

private:
   unsigned int PA[NUM_SUBKEYS];
   unsigned int SB[NUM_S_BOXES][NUM_ENTRIES];

   void GenerateSubkeys(char *Passwd);

   inline void BF_En(BFWord *x1, BFWord *x2);
   inline void BF_De(BFWord *x1, BFWord *x2);
   uint8_t crc8(uint8_t *data, int size);
};

#endif /* ___BLOWFISH_H___ */
