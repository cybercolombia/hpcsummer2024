#include <cstdio>

void upper_vowel(char &c) {
  if(c == 'a'){
    c = 'A';
  }else if(c == 'e'){
    c = 'E';
  }else if(c == 'i'){
    c = 'I';
  }else if(c == 'o'){
    c = 'O';
  }else if(c == 'u'){
    c = 'U';
  }
}
		  

int main(int argc, char *argv[]) {
  char *s = argv[1];
  char c = s[0];

  upper_vowel(c);
  printf("%c\n",c);
  
  return 0;
}
