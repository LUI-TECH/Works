#include <iostream>
#include <openssl/sha.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <fstream>
#include "stamp.h"

using namespace std;

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

// helper function for internal use only
// transforms raw binary hash value into human-friendly hexademical form
void convert_hash(const unsigned char *str, char *output, int hash_length) {
  char append[16];
  strcpy (output, "");
  for (int n=0; n<hash_length; n++) {
    sprintf(append,"%02x",str[n]);
    strcat(output, append);
  }
}

// pre-supplied helper function
// generates the SHA1 hash of input string text into output parameter digest
// ********************** IMPORTANT **************************
// ---> remember to include -lcrypto in your linking step <---
// ---> so that the definition of the function SHA1 is    <---
// ---> included in your program                          <---
// ***********************************************************
void text_to_SHA1_digest(const char *text, char *digest) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1( (const unsigned char *) text, strlen(text), hash);
  convert_hash(hash, digest, SHA_DIGEST_LENGTH);
}

/* add your function definitions here */

int leading_zeros(string digest){
  int count =0;
  for (int i = 0; i < digest.length(); i++ ){
    if ((digest[i] < '0' || digest[i] > '9') && (digest[i] < 'a' || digest[i] > 'f')){
      return -1 ;
    }
    else if (digest[i] != '0'){
      break;
    }
    count++;
  }
  return count;
}


bool file_to_SHA1_digest(string filename, char * digest){
  ifstream file(filename);
  if (!file.is_open()){
    digest[0] = 'e';
    digest[1] = 'r';
    digest[2] = 'r';
    digest[3] = 'o';
    digest[4] = 'r';
    return false;
  }
  string text;
  string temp;
  while (!file.eof()){
    file>>temp;
    text += ' ';
    text += temp;
  }
  cout<<text<<endl;
  
  text_to_SHA1_digest(text,digest);
  return true;
}