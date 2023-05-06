#include <iostream>
#include <string> //for string
#include <vector> //for vector

using namespace std;

class MOD
{
public:
    string module_name;
    string module_type;
    string mobility;
    int x;
    int y;
    string turn;
};


class MOD_f
{
public:
    string module_name;
    string module_type;
    string mobility;
    int x;
    int y;
    string turn;
    int num;
    bool is_write;
};

extern vector<MOD> module;
extern vector<MOD_f> mod_f;
