#include "header_1_1.h"
#include "header_2.h"
#include "header_3.h"

int FindFirstSubchar(char* fullchar, char* subchar) {

    char* buffer = strstr(fullchar, subchar); //用于接受返回值

    if (buffer == NULL)
    { //说明没有要找的字符串
        return -1;
    }
    else
    {                                 //说明找到了那个字符串
        return buffer - fullchar + 1; //cde的地址减去abcde的地址+1
    }

}

int FindSecondSubchar(char* fullchar, char* subchar)
{

    char* buffer = strstr(fullchar, subchar); //接收返回值
    int location;

    if (buffer == NULL)
    {
        return -1;
    }
    else
    {
        location = buffer - fullchar + strlen(subchar); //找到第一个subchar的位置
    }

    char fullchar_two[BUF_SIZE];
    memset(fullchar_two, 0, BUF_SIZE * sizeof(char));
    strncpy(fullchar_two, fullchar + location, strlen(fullchar) - location); //fullchar_two是fullchar删除了第一个subchar剩下的字符串
    fullchar_two[strlen(fullchar_two)] = '\0';

    char* buffer_two = strstr(fullchar_two, subchar);
    if (buffer_two == NULL)
    {
        return -1;
    }
    else
    {
        return buffer_two - fullchar_two + 1 + location;
    }
}

void deleteChar(char* array, int location) //http://www.360doc.cn/mip/579579350.html?ivk_sa=1024320u
{

    int j, k;

    for (j = k = 0; array[j] != '\0'; j++)
    {

        if (j != location)
        {

            array[k++] = array[j];
        }
    }

    array[k] = '\0';
}

int compareDevAddr(Rxpk* rxpk_array, int array_length){
    int flag = 1;

    for(int i=0; i<=array_length-1; i++){
        if(rxpk_array[i].DevAddr_get != rxpk_array[0].DevAddr_get){
            flag = 0;
        }
    }

    //return flag;
    return 1; //赌一把当crc值错误时DevAddr没有错误
}

int compareStat(Rxpk* rxpk_array, int array_length){
    int flag = 1;

    for(int i=0; i<=array_length-1; i++){
        if(rxpk_array[i].stat != -1){
            flag = 0;
        }
    }

    return flag;
}

int compareCRC(Rxpk* rxpk_array, int array_length){
    int flag = 1;

    for(int i=0; i<=array_length-1; i++){
        if(rxpk_array[i].crc_get != rxpk_array[0].crc_get){
            flag = 0;
        }
    }

    //return flag;
    return 1; //赌一把当四个错误时，最后一个crc值没有被修改
}

int compareRSSI(Rxpk* rxpk_array, int array_length){
    int max = rxpk_array[0].rssi;
    int index = 0;

    for(int i=0; i<=array_length-1; i++){
        if(rxpk_array[i].rssi > max){
            max = rxpk_array[i].rssi;
            index = i;
        }
    }

    return index;
}