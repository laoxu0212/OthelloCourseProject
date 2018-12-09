//
//  main.c
//  cheese
//
//  Created by Weijia Xu on 12/2/18.
//  Copyright © 2018 Weijia Xu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
// chessboard
int G[8][8];
//last change's number
int ch_iter;
//current value
double INF = (1.0/0.0);
int field;
int cur_val;
int cur_move;//current count of moves.
int dx[8] = {-1,-1,-1,0,0,1,1,1}, dy[8] = {-1,0,1,-1,1,-1,0,1};
/*
 0 3 5
 1 X 6
 2 4 7
 From 0 to 7, in X direction, the position change is dx[8]
 similarly, in Y direction, the position change is dy[8]
 */
int black = 1;
int white = (-1);
int w[8][8]={
    {2792719,-634828,290194,133277,133987,292907,-640461,2792141},
    {-639439,-534269,-105447,-185271,-185658,-96549,-533462,-631982},
    {296899,-103500,127710,-148439,-153334,123715,-102624,291699},
    {133625,-186670,-150418,0,0,-154931,-185471,134176},
    {132561,-184466,-151508,0,0,-152339,-180922,134657},
    {291285,-96017,123918,-146790,-150518,130071,-103314,296055},
    {-640795,-534037,-103774,-182132,-188671,-105241,-536888,-638507},
    {2794063,-636160,297153,132418,134495,295275,-639591,2792289}};
//The weight of each position, please check the other file that generates the weight.
typedef struct
{
    int x, y;
} Loc;
typedef struct
{
    int x, y, from;
} Change;
Change changes[2048];//a vertor that saves the change during searching temporally.
#define eps (1e-7)
#define maxdep 255
Loc BestMove[64];
int BestMoveCount[64];
Loc order[64];

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
    fflush(stdout);
}

//Place a piece, then adjust the color.
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
        if(in_board(nx, ny) && G[nx][ny] == f && (nx != x+dx[i] || ny != y+dy[i]))//it can be changed
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

int Try(int f, int x, int y, int change)//求G[x][y]是否可落f方的棋子。change=1时将改动暂存为changes数组.
{
    //    if(G[x][y]!=0)
    //        return 0;
    int nx, ny;
    int ok = 0;
    for(int i = 0;i < 8;++i)
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
                ok++;
                if(change == 1)
                {
                    G[nx][ny] = f;
                    changes[ch_iter] = (Change){nx, ny, -f};
                    ch_iter++;
                }
                else break;
                nx -= dx[i];
                ny -= dy[i];
            }
        }
    }
    if(ok)
    {
        if(change == 1)
        {
            G[x][y] = f;
            changes[ch_iter] = (Change){x, y, 0};
            ch_iter++;
        }
        return 1;
    }
    else
        return 0;
}
//input a string of vector, then change it to a board.
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
    return empty;
}

char color(int hand){
    if(hand==1)
        return 'B';
    else
        return 'W';
}

int Times;

double getVal()
{
    int cnt = 0;
    double rCount = 0, rMobility, rpMobility, rWeight = 0, rCorner;
    int mob1 = 0, mob2 = 0, pmob1 = 0, pmob2 = 0;
    int cor1 = 0, cor2 = 0;
    
    if(cur_move + 2 >= 64)
    {
        for(int i = 0;i < 8;i++)
            for(int j = 0;j < 8;j++)
                if(G[i][j] != 0)
                {
                    cnt++;
                    rCount += G[i][j];
                }
        return rCount / cnt;
    }
    for(int i = 0;i < 4;i++)
    {
        if(G[order[i].x][order[i].y] == 1)
            cor1++;
        else
            if(G[order[i].x][order[i].y] == -1)
                cor2++;
    }
    for(int i = 0;i < 8;i++)
        for(int j = 0;j < 8;j++)
            if(G[i][j] != 0)
            {
                cnt++;
                rCount += G[i][j];
                rWeight += G[i][j] * w[i][j];
            }
            else
            {
                mob1 += Try(1, i, j, 0);
                mob2 += Try(-1, i, j, 0);
                int ok1 = 0, ok2 = 0;
                for(int t = 0;t < 8;++t)
                {
                    int nx = i+dx[t], ny = j+dy[t];
                    if(in_board(nx, ny))
                    {
                        if(G[nx][ny] == white) ok1 = 1;
                        else if(G[nx][ny] == black) ok2 = 1;
                    }
                }
                pmob1 += ok1;
                pmob2 += ok2;
            }
    
    rCount /= cnt;
    rWeight /= 100000;
    //It depends on the weights, the result of the biggest weight diveded by the number above should be 1~100
    rMobility = mob1 + mob2 == 0 ? 0 : (mob1 - mob2 + 0.0) / (mob1 + mob2);
    rpMobility = pmob1 + pmob2 == 0 ? 0 : (pmob1 - pmob2 + 0.0) / (pmob1 + pmob2);
    rCorner = cor1 + cor2 == 0 ? 0 : (cor1 - cor2 + 0.0) / (cor1 + cor2);
    
    if(cur_move <= 4)
        return rCount * 30 + rWeight + rMobility * 10 + rpMobility * 10;
    else
        if(cur_move <= 32)
            return rCount * 50 + rWeight * 10 + (rMobility + rpMobility) * 10 + rCorner * 200;
        else
            return rCount * 50 + rWeight * 10 + rCorner * 300;
}

double search(int f, double a, double b, int dep, int totdep)
//alpha-beta剪枝，会被上层考虑的搜索范围为[a,b]。
//如果没有值得被考虑的结果，返回INF
{
    if(dep == totdep)
    {
        if(Times == 0)
            return a;
        double ans = f * getVal();
        if(ans > a+eps && a <= b)
            a = ans;
        Times--;
        return a;
    }
    if(a + eps >= b)
        return a;
    int iter = ch_iter;
    double tmp = a;
    
    BestMoveCount[dep] = -1;
    Loc res = BestMove[dep];
    for(int i = 0;i < 64 && a+eps < b;++i)
        if(G[order[i].x][order[i].y] == 0)
        {
            int t = Try(f, order[i].x, order[i].y, 1);
            if(t==0)
                continue;
            if(Times > 0)
                //iterative search;
                tmp = -search(-f, -b, -a, dep+1, totdep);
            else
                tmp = a;
            while(ch_iter > iter)
            {
                ch_iter--;
                G[changes[ch_iter].x][changes[ch_iter].y] = changes[ch_iter].from;
            }
            if(Times <= 0)
                return a;
            if(tmp > a+eps)
            {
                if(tmp > b)
                    break;//对方不会选择这条分支，剪枝
                res = order[i];
                a = tmp;
                BestMoveCount[dep] = i;
            }
        }
    if(BestMoveCount[dep] && Times > 0)
    {
        //Swap(BestMoveCount[dep], 4);
        BestMove[dep] = res;
    }
    return a;
}


void Put()
{
    //    int x, y;
    ch_iter = 0;
    int i;
    Times = 6000;
    for(i=1;i<= maxdep;i++)
    {
        search(field, -INF, INF, 0, i);
        if(Times == 0) break;
    }
    int k = 0;
    Loc tmp = BestMove[0];
    while(Try(field, tmp.x, tmp.y, 0)==0||G[tmp.x][tmp.y]!=0) {
        k++;
        tmp = BestMove[k];
    }
    printf("%d,%d\n",tmp.x,tmp.y);
    Place(field, tmp.x, tmp.y);
}

void Suggest()
{
    //    int x, y;
    ch_iter = 0;
    int i;
    Times = 6000;
    for(i=1;i<= maxdep;i++)
    {
        search(field, -INF, INF, 0, i);
        if(Times == 0) break;
    }
    Loc tmp = BestMove[0];
    printf("---Suggestion: %d,%d\n",tmp.x,tmp.y);
}

int cmp(const void *a, const void *b)
{
    return w[((Loc*) b)->x][((Loc*) b)->y] - w[((Loc*) a)->x][((Loc*) a)->y];
}

int test_result(){
    int sum = 0;
    for(int i=0;i<8;i++)
        for(int j=0;j<8;j++)
        {
            if(G[i][j]==1||G[i][j]==-1)
                sum+= G[i][j];
            else
                if(Try(field, i, j, 0)==1)
                    return 0;
        }
    if(sum>0)
        return 1;
    else
        if (sum<0){
            return -1;
        }
        else
            return -2;
}

int main(){
    //    char order[64];
    char inputstr[255];
    printf("%s", "Welcome to try our little A.I.\n");
    printf("%s", "Now please input the board.\n");
    scanf("%s", inputstr);
    int empty = load_board(inputstr);
    field = -(empty%2)*2+1;
    int x = 0,y = 0, result = 0;
    //here we difine balck goes first. Since we let A.I go first, we should know which color should A.I take.
    //If there are even empties, that means black go, else, white go.
    cur_move = 62 - empty;
    int it = 0;
    for(x = 0;x < 8;x++)
        for(y = 0;y < 8;y++)
        {
            order[it] = (Loc) {x, y};
            it++;
        }
    qsort(order, 64, sizeof(Loc), cmp);
    show();
    
    printf("%s %c %s", "Now the A.I shall put", color(field),"at ");
    fflush(stdout);
    Put();
    field*=-1;
    show();
    long totoal_ai_time = 0;
    while(1)
    {
        /* If you want A.I to play with itself, keep the mark below */
        printf("%s %c %s", "Please input your location of", color(field),"\n");
        Suggest();
        scanf("%d,%d", &x, &y);
        if(!Try(field, x, y, 0)||G[x][y]!=0)
        {
            printf("%s\n", "Not legal input.");
            continue;
        }
        Place(field, x, y);
        field*=-1;
        show();
        
        result = test_result();
        if(result==1)
        {
            printf("%s\n", "B wins.");
            break;
        }
        else
            if(result==-1)
            {
                printf("%s\n", "W wins.");
                break;
            }
            else
                if(result==-2)
                {
                    printf("%s\n", "Draw.");
                    break;
                }
        /* If you want to play with A.I, Unmark above */
        long begin_time = clock();
        printf("%s %c %s", "Now the A.I shall put", color(field),"at ");
        Put();
        long end_time = clock();
        field*=-1;
        show();
        totoal_ai_time += (end_time - begin_time);
        result = test_result();
        if(result==0)
            continue;
        else
            if(result==1)
            {
                printf("%s\n", "B wins.");
                break;
            }
            else
                if(result==-1)
                {
                    printf("%s\n", "W wins.");
                    break;
                }
                else
                    if(result==-2)
                    {
                        printf("%s\n", "Draw.");
                        break;
                    }
    }
    printf("%s : %ld ms \n", "Total time usage:", totoal_ai_time/(CLOCKS_PER_SEC/1000));
    return 0;
}

