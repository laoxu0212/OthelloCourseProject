#include <stdio.h>
int min(int a, int b)
{
	return a < b?  a : b;
}
int max(int a, int b)
{
	return a > b ? a : b;
}
// directions
int dx[8] = {-1,-1,-1,0,0,1,1,1}, dy[8] = {-1,0,1,-1,1,-1,0,1}
/*
0 3 5
1 X 6
2 4 7
From 0 to 7, in X direction, the position change is dx[8]
similarly, in Y direction, the position change is dy[8]
*/
// chessboard
int G[8][8];

typedef struct 
{
	int x, y, from, to;
} Change;
Change changes[100]; //to save the change during searching

void Place(int f, int x, int y)
{
	G[x][y] = f;
	cur_val += f;
	int nx, ny;
	for(int i = 0; i<8 ;++i)
	{
		// the new position
		nx = x + dx[i] + dx[i]
		ny = y + dy[i] + dy[i]
		if(nx>=0 && nx<8 && ny>=0 && ny<8)
	}

}