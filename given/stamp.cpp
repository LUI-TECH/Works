#include <iostream>
#include <openssl/sha.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
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


bool file_to_SHA1_digest(const string filename, char * digest){
  ifstream file(filename);
  if (!file.is_open()){
    error(digest);
    return false;
  }
  string text;
  string temp;
  file>>text;
  while (!file.eof()){
    file>>temp;
    text += ' ';
    text += temp;
  }

  const char *chartext = text.c_str();
  text_to_SHA1_digest(chartext,digest);
  return true;
}

void error(char * digest){
  int i;
  string errors = "error";
  for (i= 0; i < errors.length(); i++){
    digest[i] = errors[i];
  }
  while(digest[i] != NULL){
    digest[i] = NULL;
    i++;
  }
}


bool make_header(const string recipient, const string filename, char * header){

  int i;
  char hashcode[41];
  char tempcode[41];
  string temp;
  int count = 0;
  

  for (i = 0; i< recipient.length(); i++){
    header[i] = recipient[i];
  }
  header[i] = ':';
  i++;


  if (!file_to_SHA1_digest(filename, hashcode)){
    return false;
  }
  temp.assign(hashcode, 40);
  for (int j = 0; j< 40; j++){
    header[i+j] = hashcode[j];
  }
  header[i+temp.length()] = ':';

  while (count < 10000000){
    string countnum = to_string(count);
    for ( int j =0; j< countnum.length();j ++){
      header[i+j+1+40] = countnum[j];
    }   
    text_to_SHA1_digest(header, tempcode);
    temp.assign(tempcode,40);



    if (leading_zeros(temp) == 5){
      return true;
    } 
  
    count++;
  }
  return false;


}




MessageStatus check_header(const string email_address, char * header, string filename){
  char hashcode[41];
  char h[512];
  int i =0;
  
  string temp;
  for (i = 0; i< email_address.length(); i++){
    if (header[i] != email_address[i]){
      return WRONG_RECIPIENT;
    }
  }
  if (header[i] != ':'){
    return INVALID_HEADER;
  }
  file_to_SHA1_digest(filename, hashcode);
  for (int j=0; j< 40; j++){
    if (header[i+j+1] != hashcode[j]){
      return INVALID_MESSAGE_DIGEST;
    }
  }

  if (header[i+40] != ':'){
    return INVALID_HEADER;
  }
  int count = i+40+1;
  make_header(email_address, filename, h);
  while (h[count] != NULL){
    if (h[count] != header[count]){
      return INVALID_HEADER_DIGEST;
    }
    count ++;
  }

  text_to_SHA1_digest(header, hashcode);
  temp.assign(hashcode,40);
  if (leading_zeros(temp) != 5){
    return INVALID_HEADER;
  }

  return VALID_EMAIL;

}