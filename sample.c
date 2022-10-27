#include <stdio.h>


struct pop_dt {         
    int n[3];    
    long pop;           
};

int main(){
    struct pop_dt p[5];
    p[0].n[0] = 1;
    printf("%d\n",p[0].n[0]);
      return 0;
}