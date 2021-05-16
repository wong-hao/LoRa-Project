#include "header_3.h"

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