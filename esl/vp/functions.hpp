#ifndef _FUNCTIONS_HPP
#define _FUNCTIONS_HPP

#define SC_INCLUDE_FX

#include <systemc>
#include <sysc/datatypes/fx/sc_fixed.h>
#include <iostream>
#include <string.h>
#include <bitset>
#include <math.h>
#include <string>
#include <systemc>
#include <tlm>

using namespace std;

static const int DATA_WIDTH = 32;
static const int FIXED_WIDTH = 3;
static const int CHAR_LENGHT = 8;
static const int CHARS_AMOUNT = DATA_WIDTH / CHAR_LENGHT;

const int W1 = 32;
const int F1 = 3;
typedef sc_dt::sc_fixed<W1,F1> res_type;

int toInt(unsigned char *buf);

int Treshold_convert(char *buf);

void toUchar(unsigned char *buf,int val);

unsigned char Convert_to_UnsignedC(char val);

unsigned char Convert_to_UnsignedD(double val);

char Convert_to_SigendC(unsigned char val);

string complement2(string bin);

double ucharToDouble (unsigned char *buf);

void stringToChar (unsigned char *, string);

void fixToChar (unsigned char *buf, res_type d);

#endif
