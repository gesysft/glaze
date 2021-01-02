#ifndef TOOLS_H
#define TOOLS_H
#include <vector>
#include <string>
#include <map>

enum MODE {
    MODE_BASE = 0,
    MODE_GLAZE = 1
};

std::vector<std::string> split(const std::string &s, char c = ' ');
std::vector<std::vector<std::string>> read_form(const char *f);
std::map<std::string, double> percent_to_mol(std::map<std::string, double> v, std::map<std::string, double> t);
std::map<std::string, double> mol_to_c(std::map<std::string, double> v, int mode = 1);
std::map<std::string, double> mol_to_g(std::map<std::string, double> v, std::map<std::string, double> t);
std::map<std::string, double> g_to_percent(std::map<std::string, double> v);
void show(std::vector<std::map<std::string, double>> v, std::vector<std::string> s);
void show_formula(std::map<std::string, double> v);
std::map<std::string, std::map<std::string, double>> read_percent(const char *f);
std::map<std::string, double> read_periodic_table(const char *f);
std::map<std::string, double> parse_chemical_formula(const std::string &s);
std::map<std::string, double> get_molecular_weights(std::map<std::string, double> v, std::map<std::string, double> t);


double CA(std::map<std::string, double> m);
double K(std::map<std::string, double> percent);

#endif
