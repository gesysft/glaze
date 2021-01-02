#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include "tools.h"

using namespace std;

map<string, map<string, double>> read_percent(const char *f) {
    // 读取原料化学成分表
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

map<string, double> read_periodic_table(const char *f) {
    // 读取元素周期表
    map<string, double> table;

    vector<vector<string>> form = read_form(f);
    if (form.size() == 0)
        return table;
    
    for (const auto x : form) {
        table[x[0]] = atof(x[1].c_str());
    }
    return table;
}

vector<string> split(const string &s, char c) {
    vector<string> v;
    for (int i = 0, j = 0; i < s.size(); ++i) {
        while (s[i] == c && i < s.size())
            ++i;
        j = i;
        while (s[j] != c && j < s.size())
            ++j;
        if (i == j)
            break;
        v.push_back(string(s, i, j - i));
        i = j;
    }
    return v;
}

vector<vector<string>> read_form(const char *f) {
    // form is like a matrix
    vector<vector<string>> vv;
    ifstream in(f);
    
    if (!in) {
        fprintf(stderr, "open file failed: %s\n", f);
        return vv;
    }
    
    string line;
    while (getline(in, line)) {
        if (line.size() == 0)
            continue;
        if (line[0] == '#')
            continue;
        vector<string> v = split(line);
        if (v.size() > 0)
            vv.push_back(v);
    }
    return vv;
}

map<string, double> g_to_percent(map<string, double> v) {
    // 质量 -> 百分比
    map<string, double> v2;
    double total = 0.0;
    for (auto x : v) {
        total += x.second;
    }
    for (auto x : v) {
        v2[x.first] = x.second / total * 100.0;
    }
    return v2;
}

map<string, double> mol_to_g(map<string, double> v, map<string, double> t) {
    // 摩尔量 -> 质量
    map<string, double> v2;
    for (auto x : v) {
        v2[x.first] = x.second * t[x.first];
    }
    return v2;
}

map<string, double> mol_to_c(map<string, double> v, int mode) {
    // 摩尔量 -> 摩尔系数
    // mode = 0, 计算碱性氧化物的总数, 坯料
    // mode = 1, 计算中性氧化物的总数, 釉料
    map<string, double> m;
    for (auto x : v)
        m[x.first] = x.second;
    double total;
    if (mode == 1) {
        total = m["Li2O"] + m["K2O"] + m["Na2O"] + m["CaO"] + m["MgO"] + m["ZnO"] + m["BaO"];
    } else if (mode == 0) {
        total = m["Al2O3"] + m["Fe2O3"];
    }
    
    // 2. 除以总数
    map<string, double> v2;
    for (auto x : v) {
        v2[x.first] = x.second / total;
    }
    return v2;
}

void show_formula(map<string, double> v) {
    // 助熔剂 K2O Na2O Li2O BaO CaO MgO SrO PbO ZnO B2O3
    // 玻化剂 SiO2 B2O3 P2O5
    // 稳定剂 Al2O3
    // 乳浊剂 SnO2 TiO2 ZrO2
    // 着色剂 Fe2O3
    map<string, double> c;
    for (auto x : v) {
        c[x.first] = x.second;
    }

    vector<pair<string, double>> v1;
    vector<pair<string, double>> v2;
    vector<pair<string, double>> v3;
    vector<pair<string, double>> v4;

    // 碱性
    vector<string> s1 = {"K2O", "Na2O", "Li2O", "BaO", "CaO", "MgO", "PbO", "SrO", "ZnO"};
    for (auto x : s1) {
        if (c.find(x) != c.end())
            v1.push_back({x, c[x]});
    }

    // 中性
    vector<string> s2 = {"Al2O3", "Fe2O3"};
    for (auto x : s2) {
        if (c.find(x) != c.end())
            v2.push_back({x, c[x]});
    }

    // 酸性
    vector<string> s3 = {"SiO2", "B2O3", "SnO2", "TiO2", "ZrO2", "P2O5"};
    for (auto x : s3) {
        if (c.find(x) != c.end()) {
            v3.push_back({x, c[x]});
        }
    }

    // 其它
    vector<string> s4 = {"CuO"};
    for (auto x : s4) {
        if (c.find(x) != c.end())
            v4.push_back({x, c[x]});
    }

    for (int i = 0; i < v1.size() || i < v2.size() || i < v3.size() || i < v4.size(); ++i) {
        if (i < v1.size())
            printf("%-7s %-7.4f %-7s", " ", v1[i].second, v1[i].first.c_str());
        else
            printf("%-23s", " ");

        if (i < v2.size())
            printf("| %-7.4f %-7s", v2[i].second, v2[i].first.c_str());
        else
            printf("| %-15s", " ");

        if (i < v3.size())
            printf("| %-7.4f %-7s", v3[i].second, v3[i].first.c_str());
        else
            printf("| %-15s", " ");

        if (i < v4.size())
            printf("| %-7.4f %-7s", v4[i].second, v4[i].first.c_str());
        printf("\n");
    }
}


map<string, double> percent_to_mol(map<string, double> v, map<string, double> t) {
    // 百分比 -> 摩尔量
    map<string, double> v2;
    for (auto x : v) {
        v2[x.first] = x.second / t[x.first];
    }
    return v2;
}
map<string, double> get_molecular_weights(map<string, double> v, map<string, double> t) {
    // 获取摩尔质量
    map<string, double> v2;
    for (auto x : v) {
        v2[x.first] = t[x.first];
    }
    return v2;
}

void show(vector<map<string, double>> v, vector<string> s) {
    printf("%-7s ", "");   
    for (auto x : v[0]) {
        printf("%-7s ", x.first.c_str());
    }
    printf("TOTAL\n");

    double total = 0.0;
    for (int i = 0; i < v.size(); ++i) {
        total = 0.0;
        printf("%-7s ", s[i].c_str());
        for (auto x : v[i]) {
            printf("%-7.3lf ", x.second);
            total += x.second;
        }
        printf("%-7.3lf\n", total);
    }
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


