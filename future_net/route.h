#ifndef __ROUTE_H__
#define __ROUTE_H__
#include "lib_io.h"
#define MAX_VERTEX 2000  

typedef struct Node
{
	//而对于TargetNodes中的顶点只能有对应的顶点序号代替
	int ID;//该必经点在TargetNodes中的坐标
	int InDegree;//顶点的入度
	int OutDegree;//顶点的出度
	bool Demand;//是否是必须经过的顶点
	bool Delete;//该顶点是否被删除
}Node;
//图的边的定义
typedef struct EdgeNode  
{  
    int ToVertex;           	//相邻顶点
    int weight;					//当前边的权重
    int edgenum;				//当前边的编号
    struct EdgeNode * next; //下一条边  
}EdgeNode,*PEdgeNode;
//图的顶点的定义 
typedef struct  
{
	bool Demand;//是否是必须经过的顶点
	bool Delete;//该顶点是否被删除
   	int vertex;      //顶点信息  
	PEdgeNode  firstEdge;   //第一条边  
}VertexNode,*PVertexNode;
//图的定义
typedef struct   
{
    VertexNode vertexNode[MAX_VERTEX]; //顶点信息数组  
}GraphList,*PGraphList;


void search_route(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num);

void Get_Path(PGraphList pGraph,char *demand[MAX_DEMAND_NUM], int demand_num);

void TEST_printf(PGraphList pGraph);

bool InitGraph(PGraphList pGraph,char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM]);

bool GetTopoInfo(char *info,int &EdgeID,int &s,int &d,int &weight);

bool GetDemandInfo(char *info);

bool MySearch(int MyNodeNumber,Node* MyAllNodes,int MyTargetNumber,int s,int d,int deep,int length);

void CopyArray(Node* MyAllNodes,int MyNodeNumber,Node *NewAllNodes);

bool DeleteUseless(Node* NewAllNodes,int &NewNodeNumber,int s,int d);

void DeleteNode(int DeleteNodeNumber,int &NewNodeNumber,Node* NewAllNodes);

bool Dijkstra(PGraphList pGraph,int &NewNodeNumber,Node* MyAllNodes,int s,int d,int length,int* LastDistance);

bool MinDistance(PGraphList pGraph,Node* NewAllNodes,int NewNodeNumber,int NewTargetNumber,int s,int d,int length,int distance[100][2002]);

void SortDistance(Node *MyAllNodes,int MyNodeNumber,int distance[100][2002],int *Turn);

void DeleteSourceNode(int s,int &NewNodeNumber,Node* NewAllNodes,int &NewTargetNumber,int distance[100][2002],int NextSource);

void SetMinPath(int *LastDistance);

void AddMyMinPath(int *MyMinPath,int distance[100][2002],int s,int index);

bool  MyTimeOut();

void ADD_MU(PGraphList pGraph);

#endif
