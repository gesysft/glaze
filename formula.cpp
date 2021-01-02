/*
    由配方计算釉式   
*/
#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "tools.h"
#include "ptable.h"

using namespace std;

// 烧失量
int g_loi  = 0; 
int g_mode = MODE_GLAZE;
int g_ptable = 0;

map<string, string> g_config;
map<string, double> g_periodic_table;
map<string, map<string, double>> g_percent;

map<string, string> read_config(const char *f);
map<string, double> get_percent(map<string, map<string, double>> p, map<string, double> m, int mode);
void show_percent(map<string, double> m);
void percent_to_formula(map<string, double> perc);
void parse_command(string s);

int main(int argc, char *argv[]) {
    g_config = read_config("./formula.conf");
    g_periodic_table  = read_periodic_table(g_config["periodic-table-file"].c_str());
    g_percent = read_percent(g_config["percent-file"].c_str());

    int c;
    while ((c = getopt(argc, argv, "bglp")) != EOF) {
        switch (c) {
            case 'b':
                g_mode = MODE_BASE;
                break;
            case 'g':
                g_mode = MODE_GLAZE;
                break;
            case 'l':
                g_loi = 1;
                break;
            case 'p':
                g_ptable = 1;
            default:
                break;
        }
    }

    if (g_ptable == 1) {
        // 计算分子的摩尔质量
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] != '-') {
                double n = ptable(argv[i], g_periodic_table);
                printf("%s %.3lf\n", argv[i], n);
            }
        }
        return 0;
    }

    if (argc > 1) {
        string s;
        for (int i = 1; i < argc; ++i) {
            if (argv[i][0] == '-')
                continue;
            s = s + " " + argv[i];
        }
        s += " 100";
        parse_command(s);
        return 0;
    }

    return 0;
}

map<string, double> get_percent(map<string, map<string, double>> p, map<string, double> m, int mode) {
    // 总体计算几种原料的化学组成
    map<string, double> ret;
    for (auto x : m) {
        if (g_periodic_table.find(x.first) != g_periodic_table.end()) {
            if (g_loi == 0) {
                ret[x.first] +=  x.second;
            }
            else {
                ret[x.first] = ret[x.first] + (x.second / (100.0 - m["LOI"]) * 100);
                printf("%lf %lf %lf\n", x.second, m["LOI"], x.second / (100.0 - m["LOI"]) * 100);
            }
            continue;
        }

        double n = ptable(x.first, g_periodic_table);
        if (n != 0.0) {
            g_periodic_table[x.first] = n;
            ret[x.first] = ret[x.first] + (x.second / (100.0 - m["LOI"]) * 100);
            continue;
        }

        if (p.find(x.first) == p.end())
            cout << "        " << x.first << ": none" << endl;

        for (auto y : p[x.first]) {
            if (g_loi == 0) {
                ret[y.first] += y.second * x.second / 100.0;
            } else {
                ret[y.first] += (y.second / (100.0 - p[x.first]["LOI"]) * 100) * x.second / 100.0;
            }
        }
    }
    return ret;
}

void percent_to_formula(map<string, double> perc) {
    // 从化学组成计算釉式(赛格式)
    auto mol = percent_to_mol(perc, g_periodic_table);
    auto mw = get_molecular_weights(perc, g_periodic_table);
    auto c = mol_to_c(mol, g_mode);
    show({perc, mw, mol, c}, {"%", "mw", "mol", "c"});
    printf("\n");
    show_formula(c);
    //printf("        其它参数 仅供参考 不太准确\n");
    //CA(mol);
    //K(perc);
}

void show_percent(map<string, double> m) {
    set<string> name;
    for (auto x : m) {
        if (g_percent.find(x.first) != g_percent.end()) {
            for (auto y : g_percent[x.first]) {
                name.insert(y.first);
            }
        }
    }

    printf("%-7s ", "");   
    for (auto x : name) {
        printf("%-7s ", x.c_str());
    }
    printf("\n");

    for (auto x: m) {
        if (g_percent.find(x.first) != g_percent.end()) {
            printf("%-7s ", " ");
            for (auto y : name) {
                if (g_percent[x.first].find(y) != g_percent[x.first].end())  {
                    printf("%-7.3lf ", g_percent[x.first][y]);
                } else {
                    printf("%-7s ", "");
                }
            }
            printf("%-7s %lf\n", x.first.c_str(), x.second);
        }
    }
    cout << endl;
}

void parse_command(string s) {
    if (s.size() == 0)
        return;
    if (s[0] == '#')
        return;
    vector<string> v = split(s);
    map<string, double> m;
    cout << "        ";
    for (int i = 1; i < v.size(); i += 2) {
        m[v[i-1]] = atof(v[i].c_str());
        cout << v[i-1] << " " << m[v[i-1]] << " ";
    }
    cout << endl;
    map<string, double> percent = get_percent(g_percent, m, g_mode);
    show_percent(m);
    percent_to_formula(percent);
}

map<string, string> read_config(const char *f) {
    map<string, string> m;
    m["periodic-table-file"]  = "./data/periodic-table.txt";
    m["percent-file"] = "./data/jdz-percent.txt";

    vector<vector<string>> form = read_form(f);
    for (auto x : form) {
        if (x.size() < 2)
            continue;
        m[x[0]] = x[1];
        //printf("%-24s %s\n", x[0].c_str(), x[1].c_str());
    }
    return m;
}


