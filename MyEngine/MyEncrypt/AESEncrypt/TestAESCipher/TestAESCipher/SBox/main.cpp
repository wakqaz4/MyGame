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

    sbox[0] = 0x63;//00求逆为00，再异或0x63

    isbox[0x63] = 0;

    for (size_t i = 1; i < num; ++i) {

        unsigned char tmp = getMulInverse(i);//先求逆

        // b[i] = bi ^ b[(i + 4) mode 8] ^  b[(i + 5) mode 8] ^  b[(i + 6) mode 8] ^  b[(i + 7) mode 8] ^ 0x63

        tmp = tmp ^ (tmp << 4 | tmp >> 4) ^ (tmp << 3 | tmp >> 5)

                  ^ (tmp << 2 | tmp >> 6) ^ (tmp << 1 | tmp >> 7) ^ 0x63;//仿射变换，字节循环移位再异或

        if (sbox != 0)

            sbox[i] = tmp;//S盒

        if (isbox != 0)

            isbox[tmp] = i; //逆S盒

    }

}

 

/**

 * Calculate multiplicative inverse under polynomial GF(2^8).

 * @return: Multiplicative inverse of @param a.

 */

unsigned char getMulInverse(unsigned char a)

{

    unsigned char ret = 1;

    while (mul(a, ret) != 1) //两个数在GF(2^8)相乘得1，则构成互逆，这里采用的是正向穷举的办法

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

 

    for (int i = 0; i < 8; ++i) {   //循环累加b的每1位

        if (b & 0x01)   

            ret ^= a;

        if (a & 0x80) {  //，

            a <<= 1; //a先左移1位

            a ^= 0x1B; //再异或0x1B

        } else

            a <<= 1;  //,等于直接左移1位

        b >>= 1;//取b的高1位

    }

 

return ret;

}
