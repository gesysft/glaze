/*
    4边混合实验
    配釉计算
*/

#include <stdio.h>
#include <string>
#include <vector>
#include <numeric>

using namespace std;

void side4(double base, vector<double> perc, int n) {
    vector<double> a = {perc[0] / 100.0 * base, perc[1] / 100.0 * base,
        perc[2] / 100.0 * base, perc[3] / 100.0 * base};
    vector<vector<double>> v(4);

    double p = 1.0 / ((n - 1) * (n - 1));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            v[0].push_back((n - i - 1) * (n - j - 1) * a[0] * p);
            v[1].push_back((n - i - 1) * (j) * a[1] * p);
            v[2].push_back((i) * (j) * a[2] * p);
            v[3].push_back((i) * (n - j - 1) * a[3] * p);
        }
    }
    printf("BASE GLAZE = %.2lf, N = %d, PERCENT = {%.2lf%%, %.2lf%%, %.2lf%%, %.2lf%%}\n",
        base, n, perc[0], perc[1], perc[2], perc[3]);
    for (int i = 0; i < v.size(); ++i) {
        double t = accumulate(v[i].begin(), v[i].end(), 0.0);
        printf("%d: TOTAL = %-5.2lf\n", i + 1, t);
        for (int j = 0; j < v[i].size(); ++j) {
            if (((j + 1) % n) == 1)
                printf("    ");

            printf("%-7.2lf ", v[i][j]);

            if ((j + 1) % n == 0)
                printf("\n");
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    double base = 100.0;
    double n = 6;
    vector<double> percent = {100, 100, 100, 100};

    if (argc >= 3) {
        base = atof(argv[1]);
        n = atoi(argv[2]);
    }
    if (argc >= 7) {
        percent[0] = atof(argv[3]);
        percent[1] = atof(argv[4]);
        percent[2] = atof(argv[5]);
        percent[3] = atof(argv[6]);
    }
    side4(base, percent, n);
    return 0;
}
