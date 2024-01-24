#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>
#include <queue>

using namespace std;

struct node{
    int x,y;
    node(){}
    node(int x,int y):x(x),y(y){}
    bool operator< (const node &a)const {
        return x < a.x;
    }
};

struct cmp
{
    bool operator()(const node &a,const node &b)
    {
        return a.x > b.x;
    }
};
int main()
{
    vector<node> nodeList(5);
    nodeList[0] = node(1,3);
    nodeList[1] = node(2,4);
    nodeList[2] = node(2,5);
    nodeList[3] = node(3,4);
    nodeList[4] = node(5,5);

    priority_queue<node,vector<node>,cmp > q4;
    for(int i = 0 ; i < 5 ; i ++)
        q4.emplace(nodeList[i]);
    nodeList[3].x=100;
    while(!q4.empty())
        printf("%d %d\n",q4.top().x,q4.top().y),q4.pop();
    puts("\n");
    printf("%d %d",nodeList[3].x,nodeList[3].y);
    return 0;
}


