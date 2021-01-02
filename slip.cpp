/*
    配置泥浆
*/

#include <stdio.h>
#include <stdlib.h>

using namespace std;

void slip(double n, int m) {
    // 15斤泥 7斤水 10克水玻璃 5克腐植酸钠
    // 7500 3500 10 5
    const double a[] = {7500, 3500, 10, 5};

    for (int i = 0; i < 4; ++i) {
        printf("%.3lf ", n / a[m] * a[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    double number = 1000;
    int id = 0;
    
    if (argc < 2) {
        printf("usage: ./slip number id\n");
        return 0;
    }
    
    if (argc > 1)
        number = atof(argv[1]);
    
    if (argc > 2)
        id = atoi(argv[2]);

    if (id < 0 || id > 4)
        id = 0;

    slip(number, id);
    return 0;
}
