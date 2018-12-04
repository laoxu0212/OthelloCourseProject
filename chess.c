//
//  main.c
//  cheese
//
//  Created by Weijia Xu on 12/2/18.
//  Copyright © 2018 Weijia Xu. All rights reserved.
//

#include <stdio.h>
// chessboard
int G[8][8];
//last change's number
int ch_iter;
//current value
int cur_val;
int dx[8] = {-1,-1,-1,0,0,1,1,1}, dy[8] = {-1,0,1,-1,1,-1,0,1};
/*
 0 3 5
 1 X 6
 2 4 7
 From 0 to 7, in X direction, the position change is dx[8]
 similarly, in Y direction, the position change is dy[8]
 */
int in_board(int x,int y){
    if(x>=0&&x<8)
        if(y>=0&&y<8)
            return 1;
    return 0;
}
void show() {
    printf("%s", "   ");
    for(int i=0;i<8;i++)
        printf("%d%s", i," ");
    printf("%s", "\n--------------------\n");
    for(int i=0;i<8;i++)
    {
        printf("%d %s", i,"|");
        for(int j=0;j<8;j++)
            if(G[i][j]==1)
                printf("%s", "B ");
            else
                if(G[i][j]==-1)
                    printf("%s", "W ");
                else
                    printf("%s", "0 ");
        printf("%s", "\n");
    }
    printf("%s", "--------------------\n");
}
int min(int a, int b)
{
    return a < b?  a : b;
}
int max(int a, int b)
{
    return a > b ? a : b;
}

void Place(int f, int x, int y)
{
    G[x][y] = f;
    int nx, ny;
    for(int i = 0;i < 8;i++)
    {
        nx = x+dx[i];
        ny = y+dy[i];
        while(in_board(nx, ny) && G[nx][ny] == -f)
        {
            nx += dx[i];
            ny += dy[i];
        }
        if(in_board(nx, ny) && G[nx][ny] == f && (nx != x+dx[i] || ny != y+dy[i]))//可以翻转
        {
            nx -= dx[i];
            ny -= dy[i];
            while(nx != x || ny != y)
            {
                G[nx][ny] = f;
                nx -= dx[i];
                ny -= dy[i];
            }
        }
    }
}

int load_board(char inputstr[]){
    int i = 0, j = -1;
    int empty = 0;
    while(i<64)
    {
        j++;
        if(inputstr[j]=='W')
        {
            G[i/8][i%8] = -1;
            i++;
        }
        else
            if(inputstr[j]=='B')
            {
                G[i/8][i%8] = 1;
                i++;
            }
            else
                if(inputstr[j]=='0')
                {
                    G[i/8][i%8] = 0;
                    empty++;
                    i++;
                }
                else
                    continue;
    }
    if(empty%2==0)
        return 1;
    else
        return -1;
}

char color(int hand){
    if(hand==1)
        return 'B';
    else
        return 'W';
}

int main(){
//    char order[64];
    char inputstr[255];
    printf("%s", "Welcome to try our little A.I.\n");
    printf("%s", "Now please input the board.\n");
    scanf("%s", inputstr);
    int hand = load_board(inputstr);
    int x = 0,y = 0;
//here we difine balck goes first. Since we let A.I go first, we should know which color should A.I take.
//If there are even empties, that means black go, else, white go.
    show();
    while(1) {
        printf("%s %c %s", "Please input your location of", color(hand),"\n");
        scanf("%d,%d", &x, &y);
        Place(hand, x, y);
        hand*=-1;
        show();
    }
    return 0;
}

