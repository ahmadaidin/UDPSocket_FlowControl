#include <stdio.h>
#include <stdlib.h>

int main(){
FILE *fp;
fp=fopen("file.txt", "r");
unsigned char c;
unsigned char a;
int r;

r=fscanf(fp, "%c", &c);
while(r != EOF){
  printf("%c\n", c);
  a=c;
  printf("%hhu\n", a);
  r=fscanf(fp, "%c", &c);
}
  return 0;
}
