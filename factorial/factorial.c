#include <stdio.h>
#define CON 50
//函数功能：输出数组
void print(int dashu[])
{
    int i,n;
    for (i=49;i>=0;i--)
    {
        if (dashu[i]!=0) break;
    }
    for (n=i;n>=0;n--) printf("%d",dashu[n]);
    printf("\n");
}
//函数功能：计算
void calculate(int dashu[],int n21)
{
    int a21=0,i=0;
    while (i<CON-1)
    {
        dashu[i]=dashu[i]*n21+a21;
        a21=dashu[i]/10;
        dashu[i]=dashu[i]%10;
        i++;
    }
}
int main()
{
    int dashu[CON],i,n21;
    dashu[0]=1;
    for (i=1;i<CON;i++) dashu[i]=0;
    for (n21=1;n21<=40;n21++)
    {
        calculate(dashu,n21);
        print(dashu);
    }
    return 0;
}
