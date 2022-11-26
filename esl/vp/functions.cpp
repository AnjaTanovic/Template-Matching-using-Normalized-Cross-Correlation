#include "functions.hpp"

#include <iostream>

int toInt(unsigned char *buf)
{
    int val = 0;
    val += ((int)buf[0]) << 24;
    val += ((int)buf[1]) << 16;
    val += ((int)buf[2]) << 8;
    val += ((int)buf[3]);
    return val;
}

void toUchar(unsigned char *buf,int val)
{
    buf[0] = (char) (val >> 24);
    buf[1] = (char) (val >> 16);
    buf[2] = (char) (val >> 8);
    buf[3] = (char) (val);
}

unsigned char Convert_to_UnsignedC(char val)
{
    unsigned char buf = (unsigned)val;
    return buf;
} 
char Convert_to_SigendC(unsigned char val)
{
    char buf = signed(val);
    return buf;
}

char flip(char c) {return (c == '0')? '1': '0';}

double ucharToDouble (unsigned char *buf)
{

  string glue = "";
  for (int i = 0; i<CHARS_AMOUNT; ++i) // glue char array into double
    glue += bitset<CHAR_LENGHT>((int)buf[i]).to_string();
  
  double multiplier = 1;
  
  if (glue[0] == '1')
    {
      glue = complement2(glue);
      multiplier = -1;
    }
  
  double sum = 0;
  
  for (int i = 0; i < DATA_WIDTH; ++i)
  {
      sum += (glue[DATA_WIDTH-i]-'0') * pow(2.0, FIXED_WIDTH - DATA_WIDTH + i - 1);
  }
  return sum*multiplier;
  
}

string complement2(string bin)
{
  int n = bin.length();
  int i;

  string ones, twos;
  ones = twos = "";

  //  for ones complement flip every bit
  for (i = 0; i < n; i++)
    ones += flip(bin[i]);

  //  for two's complement go from right to left in
  //  one's complement and if we get 1 make, we make
  //  them 0 and keep going left when we get first
  //  0, make that 1 and go out of loop
  twos = ones;
  for (i = n - 1; i >= 0; i--)
    {
      if (ones[i] == '1')
        twos[i] = '0';
      else
        {
          twos[i] = '1';
          break;
        }
    }

  if (i == -1)
    twos = '1' + twos;

  return twos;
}

void fixToChar(unsigned char *c, res_type d)
{
  stringToChar(c,d.to_bin());
}

void stringToChar (unsigned char *buf, string s)
{
  s.erase(0,2); // remove "0b"
  s.erase(FIXED_WIDTH,1); // remove the dot
  char single_char[CHAR_LENGHT];
  for (int i = 0; i < CHARS_AMOUNT; ++i)
    {
      s.copy(single_char,CHAR_LENGHT,i*CHAR_LENGHT); // copy 8 letters (0s and 1s) to char array
      int char_int = stoi(single_char, nullptr, 2); // binary string -> int
      buf[i] = (unsigned char) char_int;
    }
}

int Treshold_convert(char *buf)
{
  int val = 0;
  val += (((int)buf[0]) - '0') * 100;
  val += (((int)buf[1]) - '0') * 10;
  val += (((int)buf[2]) - '0') * 1;

  return val;
}
