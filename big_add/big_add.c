#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define N 50
short addition(char A[],char B[],char C[],short m,short n)
{
    short i,temp=0;
    char c;
    for(i=m-1; i>0; i--)
    {
        if(i>=m-n) c=A[i]+B[i-(m-n)]-96;
        else c=A[i]-48;
        if (c<0||c>18)
        {
            printf("ERROR!");
            return 0;
        }
        C[i]=(c+temp)%10;
        temp=(c+temp)/10;
    }
    if ((m-n)!=0) C[0]=A[0]-48+temp;
    else C[0]=A[0]+B[0]-96+temp;
    return m;
}
int main()
{
    printf("Number:210320621\nSubject No.7---Program No.1\n");
    char A[N],B[N],C[N];
    short i,lena,lenb,len;
    printf("Please input the numbers:");
    scanf("%s%s",A,B);
    lena=strlen(A);
    lenb=strlen(B);
    if (lena>=lenb) len=addition(A,B,C,lena,lenb);
    else len=addition(B,A,C,lenb,lena);
    for (i=0; i<len; i++) printf("%d",C[i]);
    return 0;
}
