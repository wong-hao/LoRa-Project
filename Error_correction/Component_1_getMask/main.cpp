#include "header_1_3.h"

int main()
{
    uint8_t Hexstring1[BUF_SIZE] = "80041304268000000232451D253CC028DA5CAFD77B54491BA995BDC6294900899F461C085C88\0"; //m's
    uint8_t Binarystring1[BUF_SIZE] = { 0 };
    uint8_t Hexstring2[BUF_SIZE] = "40041304268000000232451D253C0028DA5CAFD77B54491BA995BDC6294900899F461C085C88\0"; //m'r
    uint8_t Binarystring2[BUF_SIZE] = { 0 };
    uint8_t Hexstring3[BUF_SIZE] = { 0 };
    uint8_t Binarystring3[BUF_SIZE] = { 0 }; //Merged error mask / Ambiguity vectors / Va

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : uint8_t转char ---------------------- */ //https://bbs.csdn.net/topics/390141308

    char* Hexstring11 = (char*)Hexstring1;
    char* Binarystring11 = (char*)Binarystring1;
    char* Hexstring21 = (char*)Hexstring2;
    char* Binarystring21 = (char*)Binarystring2;
    char* Hexstring31 = (char*)Hexstring3;
    char* Binarystring31 = (char*)Binarystring3;

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 十六进制字符串转二进制字符串 ---------------------- */
    Hex2Bin(Hexstring11, Binarystring11);
    Hex2Bin(Hexstring21, Binarystring21);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 二进制字符串异或 ---------------------- */
    if (OZ_bin_xor(Binarystring11, Binarystring21, Binarystring31) != 0)
    {
        printf("函数出错！\n");
        return 1;
    }
    puts(Binarystring31);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : 二进制字符串转十六进制字符串 ---------------------- */

    Bin2Hex(Binarystring31, Hexstring31);
    //puts(Hexstring31);


    /* -------------------------------------------------------------------------- */
    /* --- STAGE : APC ---------------------- */

    char copy1[BUF_SIZE] = "1101";
    char copy2[BUF_SIZE] = "1000";
    char copy3[BUF_SIZE] = "1011";
    char output1[BUF_SIZE];
    char output2[BUF_SIZE];

    majorityTry(copy1, copy2, copy3, output1);
    majorityMask(copy1, copy2, copy3, output2);

    printf("Possible Candidate of APC is: %s\n", output1);
    printf("Mask of APC is: %s\n", output2);

    /* -------------------------------------------------------------------------- */
    /* --- STAGE : MPC ---------------------- */

    mpc(copy1, copy2, copy3);

    return 0;
}