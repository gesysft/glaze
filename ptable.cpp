#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "tools.h"
#include "lex.yy.c"

using namespace std;

#define debug(fmt, args...) fprintf(stderr, fmt, ##args)

double ptable(const string &in, map<string, double> periodic_table) {
    int c;
    int state = 1;
    char buf[1024];
    int i = 0;
    map<string, double> m;
    vector<pair<string, double>> v;

    string input = in + " ";
    for (int j = 0; j < input.size(); ++j) {
        c = input[j];
        if (!isascii(c)) {
            break;
        }
        buf[i++] = c;
        //printf("state: %d '%c' %d\n", state, c, yy_nxt[state][c]);
        if ((state = yy_nxt[state][c]) < 0) {
            buf[i-1] = '\0';
            //printf("accept %d: %s\n", yy_accept[-state], buf);
            if (yy_accept[-state] == 119) {
                // number
                v[v.size() - 1].second = atof(buf);
            }
            else if (yy_accept[-state] == 120) {
                // not accept
            } else {
                // accept
                v.push_back({buf, 1});
            }
            i = 0;
            state = 1;
            --j;
        }
    }

    double total  = 0.0;
    stringstream in2;
    for (auto x : v) {
        in2 << x.first;
        if (x.second != 1)
            in2 << x.second;
        total += periodic_table[x.first] * x.second;

    }
    if (in2.str().size() != in.size())
        return 0.0;
    return total;
}
