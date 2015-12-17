/**

 * Calculate S-Box (Substitute Bytes) and inverse S-Box (Inverse Substitute Bytes) under polynomial GF(2^8).

 * @param sbox: Array which holds the sbox table.

 * @param isbox: Array which holds the inverse sbox table.

 * @param num: Size of the array (should be 256).

 */
#include <iostream>

unsigned char getMulInverse(unsigned char a);
unsigned char mul(unsigned char a, unsigned char b);
void getSbox(unsigned char sbox[], unsigned char isbox[], size_t num);

int  main()
{
	unsigned char sbox[256];
	unsigned char isbox[256];
	getSbox(sbox, isbox, 256);
	for (int i = 0; i< 256; i++)
	{
		printf("%X ", sbox[i]);
		if ((i+1) % 16 == 0)
		{
			printf("\n");
		}
	}
	return 0;
}
void getSbox(unsigned char sbox[], unsigned char isbox[], size_t num)

{

    sbox[0] = 0x63;//00����Ϊ00�������0x63

    isbox[0x63] = 0;

    for (size_t i = 1; i < num; ++i) {

        unsigned char tmp = getMulInverse(i);//������

        // b[i] = bi ^ b[(i + 4) mode 8] ^  b[(i + 5) mode 8] ^  b[(i + 6) mode 8] ^  b[(i + 7) mode 8] ^ 0x63

        tmp = tmp ^ (tmp << 4 | tmp >> 4) ^ (tmp << 3 | tmp >> 5)

                  ^ (tmp << 2 | tmp >> 6) ^ (tmp << 1 | tmp >> 7) ^ 0x63;//����任���ֽ�ѭ����λ�����

        if (sbox != 0)

            sbox[i] = tmp;//S��

        if (isbox != 0)

            isbox[tmp] = i; //��S��

    }

}

 

/**

 * Calculate multiplicative inverse under polynomial GF(2^8).

 * @return: Multiplicative inverse of @param a.

 */

unsigned char getMulInverse(unsigned char a)

{

    unsigned char ret = 1;

    while (mul(a, ret) != 1) //��������GF(2^8)��˵�1���򹹳ɻ��棬������õ���������ٵİ취

        ++ret;

 

    return ret;

}

 

/**

 * Multiply two numbers under polynomial GF(2^8).

 * @return: The product of @param a and @param b.

 */

unsigned char mul(unsigned char a, unsigned char b)

{

    unsigned char ret = 0;

 

    for (int i = 0; i < 8; ++i) {   //ѭ���ۼ�b��ÿ1λ

        if (b & 0x01)   

            ret ^= a;

        if (a & 0x80) {  //��

            a <<= 1; //a������1λ

            a ^= 0x1B; //�����0x1B

        } else

            a <<= 1;  //,����ֱ������1λ

        b >>= 1;//ȡb�ĸ�1λ

    }

 

return ret;

}
