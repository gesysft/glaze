/*
    根据摩尔数计算质量百分数
*/
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include "tools.h"

using namespace std;

map<string, double> g_periodic_table;

void parse_command(string);

vector<map<string, double>> read_mol(const char *f, int begin = 0) {
    vector<map<string, double>> vv;
    vector<vector<string>> form = read_form(f);
    for (auto v : form) {
        map<string, double> table;
        for (int i = 1 + begin; i < v.size(); i += 2) {
            table[v[i-1]] = atof(v[i].c_str());
        }
        if (table.size() > 0) {
            vv.push_back(table);
        }
    }
    return vv;
}

int parse_file(string f) {
    vector<map<string, double>> mol = read_mol(f.c_str(), 1);
    for (auto m : mol) {
        map<string, double> g = mol_to_g(m, g_periodic_table);
        map<string, double> perc = g_to_percent(g);
        //show({m, g, perc}, {"mol", "g", "%"});
        for (auto x : perc) {
            if (x.second != 0.0)
                printf("%s %.2lf ", x.first.c_str(), x.second);
        }
        printf("\n");
    }
    return 0;
}

// 从釉式计算化学组成
void parse_command(string s) {
    if (s.size() == 0)
        return;
    if (s[0] == '#')
        return;
    vector<string> v = split(s);
    map<string, double> mol;
    for (int i = 1; i < v.size(); i += 2) {
        mol[v[i]] =  atof(v[i-1].c_str());
    }
    if (mol.size() < 1)
        return;

    map<string, double> g = mol_to_g(mol, g_periodic_table);
    map<string, double> perc = g_to_percent(g);
    show({mol, g, perc}, {"mol", "g", "%"});
    for (auto x : perc)
        cout << x.first << " " << x.second << " ";
    cout << endl;
}
    
int main(int argc, char *argv[]) {
    g_periodic_table  = read_periodic_table("../data/periodic_table.txt");

    if (argc > 1) {
        string s;
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-')
                continue;
            s = s + " " + argv[i];
        }
        parse_command(s);
        return 0;
    } else {
        printf("usage: ./percent 0.18 Al2O3 0.18 K2O 1.078 SiO2\n");
        return 0;
    }

    if (argc > 1) {
        string f = argv[1];
        parse_file(f);
        return 0;
    }
    return 0;
}
