#include "utils_priv.h"


char converted_string[17];  // LCD 2x16 can take 16 characters in line

//--------------------------------------------------------------------------------------------------
char* convert_num_to_str(uint16_t digital){
  char tmp_x;
  uint8_t n = 0;
  uint8_t i = 0;
  uint8_t k = 0;
  uint8_t str_len = 0;

  memset(converted_string, 0, 17);

  //define array size to store string
  //NOTE: value + \0 symbol
  if(9 - digital >= 0)
      str_len = 2;
  else if(99 - digital >= 0)
      str_len = 3;
  else if(999 - digital >= 0)
      str_len = 4;
  else if(9999 - digital >= 0)
      str_len = 5;
  else if(99999 - digital >= 0)
      str_len = 6;

  // prepare temporary string
  char tmp[str_len];
  n = str_len;
  while(n){
      tmp[--n] = 0;
  }

  //
  do{
    tmp_x = digital%10;
    tmp[i] = (tmp_x + 48);
    ++i;

  }
  while(((digital /= 10) != 0) && (i < str_len));
  tmp[i] = '\0';

  for(; k < i; k++){
    converted_string[k] = tmp[i-k-1];
  }
  converted_string[k+1] = tmp[i];
  return converted_string;
}


