//
//  main.cpp
//  Revise
//
//  Created by Weijia Xu on 12/2/18.
//  Copyright © 2018 Weijia Xu. All rights reserved.
//

#include <iostream>
#include <signal.h>
#include <ctime>
using namespace std;
int t;
void func(int x)
{
    fprintf(stderr, "cur_%d\n", t);
}

#define INF 0x3f3f3f3f
#define black 1
#define white (-1)
const int maxn = 8;
int dx[8] = {-1,-1,-1,0,0,1,1,1}, dy[8] = {-1,0,1,-1,1,-1,0,1};//方向
int G[maxn][maxn];
inline bool in_board(int x)
{
    return x < maxn && x >= 0;
}
inline bool in_board(int x, int y)
{
    return in_board(x) && in_board(y);
}
int field, w[maxn][maxn], w_black[maxn][maxn], w_s[maxn][maxn], str[maxn][maxn];

struct Loc
{
    int x, y;
};

bool Put(int f)
{
    bool ok = false;
    Loc lst[128];
    int lcnt = 0;
    for(int x = 0;x < maxn;x++)
        for(int y = 0;y < maxn;y++) if(G[x][y] == 0)
        {
            ok = false;
            for(int i = 0;i < 8 && !ok;i++)
            {
                int nx = x+dx[i], ny = y+dy[i];
                if(!in_board(nx, ny) || G[nx][ny] != -f) continue;
                
                while(in_board(nx, ny) && G[nx][ny] == -f)
                {
                    nx += dx[i];
                    ny += dy[i];
                }
                if(in_board(nx, ny) && G[nx][ny] == f)
                    ok = true;
            }
            if(ok) lst[lcnt++] = (Loc){x, y};
        }
    if(lcnt == 0) return false;
    
    random_shuffle(lst, lst+lcnt);
    lcnt = rand() % lcnt;
    int x = lst[lcnt].x, y = lst[lcnt].y;
    str[x][y] = f;
    for(int i = 0;i < 8;++i)
    {
        int nx = x+dx[i], ny = y+dy[i];
        if(!in_board(nx, ny) || G[nx][ny] != -f)
            continue;
        
        while(in_board(nx, ny) && G[nx][ny] == -f)
        {
            nx += dx[i];
            ny += dy[i];
        }
        
        if(in_board(nx, ny) && G[nx][ny] == f)
        {
            nx -= dx[i];
            ny -= dy[i];
            while(nx != x || ny != y)
            {
                G[nx][ny] = f;
                nx -= dx[i];
                ny -= dy[i];
            }
            G[x][y] = f;
        }
    }
    return true;
    
}

void End()
{
    int s = 0;
    for(int i = 0;i < maxn;i++)
        for(int j = 0;j < maxn;j++)
            s += G[i][j];
    if(s > 0)
    {
        for(int i = 0;i < maxn;i++)
            for(int j = 0;j < maxn;j++)
            {
                w[i][j] += str[i][j];
                w_s[i][j] += G[i][j];
                if(str[i][j] == black)
                    w_black[i][j]++;
            }
    }
    else if(s < 0)
    {
        for(int i = 0;i < maxn;i++)
            for(int j = 0;j < maxn;j++)
            {
                w[i][j] -= str[i][j];
                w_s[i][j] -= G[i][j];
                if(str[i][j] == black)
                    w_black[i][j]--;
            }
    }
    FILE *fp = fopen("output.txt", "w");
    fprintf(fp, "w[%d][%d]={\n", maxn, maxn);
    for(int i = 0;i < maxn;i++)
    {
        fprintf(fp, "{");
        for(int j = 0;j < maxn;j++)
            fprintf(fp, "%d%c", w[i][j], ",}"[j+1==maxn]);
        fprintf(fp, "%c\n", ",}"[i+1==maxn]);
    }
    fprintf(fp, "\n");
    
    fprintf(fp, "w_s[%d][%d]={\n", maxn, maxn);
    for(int i = 0;i < maxn;i++)
    {
        fprintf(fp, "{");
        for(int j = 0;j < maxn;j++)
            fprintf(fp, "%d%c", w_s[i][j], ",}"[j+1==maxn]);
        fprintf(fp, "%c\n", ",}"[i+1==maxn]);
    }
    fprintf(fp, "\n");
    
    fprintf(fp, "w_black[%d][%d]={\n", maxn, maxn);
    for(int i = 0;i < maxn;i++)
    {
        fprintf(fp, "{");
        for(int j = 0;j < maxn;j++)
            fprintf(fp, "%d%c", w_black[i][j], ",}"[j+1==maxn]);
        fprintf(fp, "%c\n", ",}"[i+1==maxn]);
    }
    fprintf(fp, "\n");
    fclose(fp);
}

int main()
{
    // time_t start = time(0);
    srand(time(NULL));
    signal(SIGHUP, func);
    memset(G, 0, sizeof(G));
    int x = maxn / 2, y = x-1;
    int S = 100000;
    field = black;
    for(t = 1;t <= S;++t)
    {
        cout<<t<<endl;
        bool ok = true;
        int f = field;
        memset(G, 0, sizeof(G));
        memset(str, 0, sizeof(str));
        G[x][y] = G[y][x] = black;
        G[x][x] = G[y][y] = white;
        while(true)
        {
            if(!Put(f))
            {
                if(!ok)
                {
                    End();
                    break;
                }
                else 
                    ok = false;
            }
            else 
                ok = true;
            f = -f;
        }
        field = -field;
    }
    // time_t over = time(0);
    // cout<<over-start<<endl;
    return 0;
}

