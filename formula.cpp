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
map<string, map<string, double>> read_percent(const char *f);
map<string, double> get_percent(map<string, map<string, double>> p, map<string, double> m, int mode);
void show_percent(map<string, double> m);
void percent_to_formula(map<string, double> perc);
void parse_command(string s);
double CA(map<string, double> m);
double K(map<string, double> percent);

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

map<string, map<string, double>> read_percent(const char *f) {
    map<string, map<string, double>> percent;
    vector<vector<string>> form = read_form(f);
    for (int i = 0; i < form.size(); ++i) {
        if (form.size() < 1)
            continue;
        map<string, double> m;
        for (int j = 2; j < form[i].size(); j += 2) {
            m[form[i][j-1]] = atof(form[i][j].c_str());
        }
        percent[form[i][0]] = m;
    }
    return percent;
}

map<string, double> get_percent(map<string, map<string, double>> p, map<string, double> m, int mode) {
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

double CA(map<string, double> m) {
    // 酸度系数
    // 硬质瓷(RO+R2O).(0.5 ~ 1.4)Al2O3.(5 ~ 12)SiO2
    // C.A = 1.8 ~ 2.5 烧成温度 1320 ~ 1450
    // 软质瓷(RO+R2O).(0.3~0.6)Al2O3.(3~4)SiO2
    // C.A = 1.4 ~ 1.6 烧成温度 1250 ~ 1280
    set<string> RO2{"SiO2", "TiO2", "B2O3", "As2O3", "P2O5", "Sb2O5", "Sb2O3"};
    set<string> R2O{"K2O", "Na2O", "Li2O", "Cu2O"};
    set<string> RO{"CaO", "MgO", "PbO", "ZnO", "BaO", "FeO", "MnO", "CdO"};
    set<string> R2O3{"Al2O3", "Fe2O3", "Mn2O3", "Cr2O3"};

    double a = 0.0;
    double b = 0.0;
    double ca = 0.0;
    for (auto x : m) {
        if (RO2.find(x.first) != RO2.end()) {
            a += x.second;
        } else if (RO.find(x.first) != RO.end()) {
            b += x.second;
        } else if (R2O.find(x.first) != R2O.end()) {
            b += x.second;
        } else if (R2O3.find(x.first) != R2O3.end()) {
            b += 3 * x.second;
        }
    }
    if (b != 0.0)
        ca = a / b;

    string temp = "";
    if (ca < 1.4)
        temp = " < 1250";
    else if (ca >= 1.4 && ca < 1.6)
        temp = " 1250 ~ 1280";
    else if (ca >= 1.6 && ca < 1.8)
        temp = " 1280 ~ 1320";
    else if (ca >= 1.8 && ca < 2.5)
        temp = " 1320 ~ 1450";
    else if (ca >= 2.5)
        temp = " > 1450";

    printf("        硬度系数 C.A = %lf 烧成温度 %s\n", ca, temp.c_str());
    return ca;
}

double K(map<string, double> percent) {
    // 熔融温度
    static map<string, double> A = {{"NaF", 1.3}, {"B2O3", 1.25}, {"K2O", 1.0}, {"Na2O", 1.0}, {"CaF2", 1.0},
        {"CoO", 0.8}, {"NiO", 0.8}, {"MnO2", 0.8}, {"MnO", 0.8}, {"Na3SbO3", 0.65}, {"MgO", 0.6}, {"ZnO", 1.0},
        {"BaO", 1.0}, {"PbO", 0.8}, {"AlF3", 0.8}, {"FeO", 0.8}, {"Fe2O3", 0.8}, {"Sb2O5", 0.6}, {"Cr2O3", 0.6},
        {"Sb2O3", 0.5}, {"CaO", 0.5}};
    static map<string, double>  B = {{"SiO2", 1.0}, {"Al2O3", 1.2}, {"SnO2", 1.67}, {"P2O5", 1.9}};
    static map<double, double> Kt = {{0.5, 1025}, {0.4, 1100}, {0.3, 1200}, {0.2, 1300}, {0.1, 1450}};
    double a = 0.0;
    double b = 0.0;
    double k = 0.0;
    for (auto x : percent) {
        if (A.find(x.first) != A.end()) {
            a += A[x.first] * x.second;
        }
        else if (B.find(x.first) != B.end()) {
            b += B[x.first] * x.second;
        }
    }
    if (b != 0)
        k = a / b;
    string temp;
    double t = 0.0;
    if (k < 0.1)
        temp = " > 1450";
    else if (k >= 0.1 && k < 0.2) {
        temp = " 1300 ~ 1450";
        t = -1500 * k + 1600;
    }
    else if (k >= 0.2 && k < 0.3) {
        temp = " 1200 ~ 1300";
        t = -1000 * k +  1500;
    }
    else if (k >= 0.3 && k < 0.4) {
        temp = " 1100 ~ 1200";
        t = -1000 * k + 1500;
    }
    else if (k >= 0.4 && k < 0.5) {
        temp = " 1025 ~ 1100";
        t = -750 * k + 1400;
    }
    else if (k >= 0.5) {
        temp = " < 1025";
    }
    printf("        熔融温度   K = %lf 烧成温度 %s (%lf)\n", k, temp.c_str(), t);
    return k;
}

// 从化学组成计算釉式(赛格式)
void percent_to_formula(map<string, double> perc) {
    auto mol = percent_to_mol(perc, g_periodic_table);
    auto mw = get_molecular_weights(perc, g_periodic_table);
    auto c = mol_to_c(mol, g_mode);
    show({perc, mw, mol, c}, {"%", "mw", "mol", "c"});
    printf("\n");
    show_formula(c);

    //printf("        其它参数 仅供参考 不太准确\n");
    //CA(mol);
    K(perc);
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
    m["periodic-table-file"]  = "../data/periodic_table.txt";
    m["percent-file"] = "../data/jdz-percent.txt";

    vector<vector<string>> form = read_form(f);
    for (auto x : form) {
        if (x.size() < 2)
            continue;
        m[x[0]] = x[1];
        //printf("%-24s %s\n", x[0].c_str(), x[1].c_str());
    }
    return m;
}


