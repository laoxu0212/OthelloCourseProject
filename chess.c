#include <stdio.h>


int main()
{
	char statu[64];
    printf("%s", "Welcome to try our little A.I.");
    scanf("%s", order);
    for(int i=0;i<64;i++)
    {
        if(statu[i]=="W")
            G[i/8][i%8] = -1;
        else if(statu[i]=="B")
                G[i/8][i%8] = 1;
            else
                if(statu[i]=="0")
                    G[i/8][i%8] = 0;
    }
	return 0;
}