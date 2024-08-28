#include <libepc.h>
#include <stdlib.h>
#include <stdio.h>
void llmultiply(unsigned long long int l1,
                unsigned long long int l2,
                unsigned char *result);

struct test_case {
  unsigned long long int a;
  unsigned long long int b;
  unsigned long long int rh;
  unsigned long long int rl;
};

struct test_case cases[6] = {

  { 0x0000111122223333ULL, 0x0000555566667777ULL,
    0x0000000005B061D9ULL, 0x58BF0ECA7C0481B5ULL },

  { 0x3456FEDCAAAA1000ULL, 0xEDBA00112233FF01ULL,
    0x309A912AF7188C57ULL, 0xE62072DD409A1000ULL },

  { 0xFFFFEEEEDDDDCCCCULL, 0xBBBBAAAA99998888ULL,
    0xBBBB9E2692C5DDDCULL, 0xC28F7531048D2C60ULL },

  { 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL,
    0xFFFFFFFFFFFFFFFEULL, 0x0000000000000001ULL },
  
  { 0x00000001FFFFFFFFULL, 0x00000001FFFFFFFFULL,
    0x0000000000000003ULL, 0xFFFFFFFC00000001ULL },

  { 0xFFFEFFFFFFFFFFFFULL, 0xFFFF0001FFFFFFFFULL,
    0xFFFE0002FFFDFFFEULL, 0x0001FFFE00000001ULL }
};

void PutUnsignedLongLong(unsigned long long int* ulli)
{
  unsigned long int* uli = (unsigned long int*)ulli; 
  PutUnsigned(uli[1], 16, 8);
  PutUnsigned(uli[0], 16, 8); 
}

#define E_HIGH(x, y) (((x*y) >> 32) & 0xFFFFFFFF)

#define E_LOW(x, y) ((x*y) & 0xFFFFFFFF)

void cmult(unsigned long long int l1, unsigned long long int l2, unsigned char* return_value)
{
	unsigned long long int AH = E_HIGH(l1,1);
	unsigned long long int AL = E_LOW(l1,1);
	unsigned long long int BH = E_HIGH(l2,1);
	unsigned long long int BL = E_LOW(l2,1);
	unsigned int* result = (unsigned int*) return_value;
	result[2] = 0;
	result[3] = 0;
	result[1] = 0;
	result[0] = 0;

	result[0] += E_LOW(AL, BL);
	result[1] += E_HIGH(AL, BL) + E_LOW(AH, BL);
	

	if(result[1] < E_LOW(AH, BL))
		result[2]++;

	result[1] += E_LOW(AL, BH);
	if(result[1] < E_LOW(AL, BH))
		result[2]++;

	
	unsigned int temp = result[2];	
	result[2] += E_HIGH(AL, BH);
	if(result[2] < temp)
	{
		result[3]++;
	}
	temp = result[2];	
	result[2] +=E_HIGH(AH, BL);
	if(result[2] < temp)
	{
		result[3]++;
	}
	temp = result[2];
	result[2] += E_LOW(AH, BH);

	if(result[2] < temp)
	{
		result[3]++;
	}
	result[3] += E_HIGH(AH, BH);
}

int main(int argc, char *argv[])
{
  unsigned char result[16];
  int i;
  
  ClearScreen(0x07);
  SetCursorPosition(0, 0);
  QWORD64 count = 0;
  for (i = 0; i < 6; ++i)
  {
    PutString("Test : ");
    PutUnsignedLongLong(&cases[i].a);
    PutString(" * ");
    PutUnsignedLongLong(&cases[i].b); 
    PutString("\r\n");
   
    PutString("    == ");
    PutUnsignedLongLong(&cases[i].rh);
    PutUnsignedLongLong(&cases[i].rl);
    PutString("\r\n");
    
    QWORD64 temp = CPU_Clock_Cycles();
    cmult(cases[i].a, cases[i].b, result);
    count += CPU_Clock_Cycles() - temp;   
    
    PutString("Result ");
    PutUnsignedLongLong(&result[8]);
    PutUnsignedLongLong(&result[0]); 
    PutString("\r\n");

    PutString("\r\n");
  }
    PutString("CPU Clock Cycles: ");
    PutUnsigned(count, 10, 10);
    /*
        cmult -O0:  0001217708
        cmult -O1:  0000403470
        cmult -O2:  0000274520
        cmult -O3:  0000341934
        asm:        0000179352
        
        The assembler code is still the fastest after testing different optimizations for the c version. 
        The result came as a surprise as I assumed that the optimized compile of the c code would be better than my assembler version. 
        I would guess this is due to a poorly written c function which has unneccessary statements which the compiler did not remove.
        However, this also shows that the optimized compile of c code isn't always preferable to human written assembler code.
    */
  return 0;
}

