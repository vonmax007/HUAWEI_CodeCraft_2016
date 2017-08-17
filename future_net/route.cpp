
#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h> 

#define MAX_Weight  9999999//该距离为不可达
#define ADD_Weight  100000

struct timeval t_start,t_end;
struct timeval return_start,return_end;
//Edge[0][600][600] 用来存储边的序号；
//Edge[1][600][600]用来存储边的权值；
//由于边的端点是整数表示的，所以Edge[1][i][j]表示顶点i到j的权值，
//Edge[0][i][j]表示顶点i到j的边的序号
GraphList  Graph;

Node AllNodes[2000];//存放所有节点,和原来一样保持不变
int AllUsedNode[2000];//用来存放所有图中存在的顶点，为了方便存储跳跃的顶点信息
int Edge[2][2000][2000];
int NodeNumber=0;//总共使用的顶点的个数，也即是边对应的邻接矩阵的实际使用长度
int Result=9999999;//12000是路径长度不可能达到的一个上限
int TargetNodes[100];//存放所有必须经过的节点的ID
int TargetNumber;//存放题目中必须经过的节点的个数
int S;//起点
int D;//终点

int MinPathLength=0;
int MinPath[2000];//存的是当前找到的最优路径中的节点，顺序为逆序，和MyMinPath[600]的表示方式不一样，
int MyMinPath[2000];//用于递归的时候临时使用，存的都是本次路径中其前驱节点
int StoreDistances[100][100][2002];
int Distory=0;

//你要完成的功能总入口

bool MyTimeOut()
{
	gettimeofday(&t_end, NULL);
	if((1000000*(t_end.tv_sec-t_start.tv_sec)+(t_end.tv_usec-t_start.tv_usec))>9700000)
		return true;
	else
		return false;
}
bool Distory_TimeOut()
{
	gettimeofday(&return_end, NULL);
	if((1000000*(return_end.tv_sec-return_start.tv_sec)+(return_end.tv_usec-return_start.tv_usec))>1000000)
		return true;
	else
		return false;
}
void search_route(char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM], int demand_num)
{
	if(InitGraph(&Graph,topo,edge_num,demand)==false){
		return ;}
	//TEST_printf(&Graph);
	Get_Path(&Graph,demand,0);
	//TEST_printf(&Graph);

	for(int i=0; i<TargetNumber; i++)
		AllNodes[TargetNodes[i]].Demand=false;
	GetDemandInfo(demand[1]);
	for(int i=0;i<2000;i++){
		MinPath[i]=-1;
		MyMinPath[i]=-1;
	}
	MinPathLength=0;
	Result=9999999;
	Distory=0;

	//TEST_printf(&Graph);
	Get_Path(&Graph,demand,1);

	return ;
}

void Get_Path(PGraphList pGraph,char *demand[MAX_DEMAND_NUM], int demand_num)
{
	int same=0;
	gettimeofday(&t_start, NULL);
	printf("**********PATH*************\n");
	printf("起点：%d,终点：%d,必经点数：%d,顶点数：%d\n",S,D,TargetNumber,NodeNumber);
	printf("必经点: ");
	for(int i=0; i<TargetNumber; i++)
		printf("%d|",TargetNodes[i]);
	printf("\n");
	MySearch(NodeNumber,AllNodes,TargetNumber,S,D,1,0);
	if(Result==MAX_Weight){
		printf("$$\n");
		ADD_MU(&Graph);
		MySearch(NodeNumber,AllNodes,TargetNumber,S,D,1,0);
		ADD_MU(&Graph);
	}

	if(demand_num==0){
		PEdgeNode p;
		p=pGraph->vertexNode[S].firstEdge;
		printf("第一条路径edgenum:%d|", Edge[0][S][MinPath[MinPathLength-1]]);
		record_result(WORK_PATH,Edge[0][S][MinPath[MinPathLength-1]]);
		Edge[1][S][MinPath[MinPathLength-1]]+=ADD_Weight;
		while(p!=NULL){
			if(p->edgenum==Edge[0][S][MinPath[MinPathLength-1]]){
				p->weight+=ADD_Weight;
				break;
			}
			p=p->next;
		}
		for(int i=MinPathLength-1,j=1;i>0;i--,j++){
			printf("%d|", Edge[0][MinPath[i]][MinPath[i-1]]);
			record_result(WORK_PATH,Edge[0][MinPath[i]][MinPath[i-1]]);
			Edge[1][MinPath[i]][MinPath[i-1]]+=ADD_Weight;
			p=pGraph->vertexNode[MinPath[i]].firstEdge;
			while(p!=NULL){
				if(p->edgenum==Edge[0][MinPath[i]][MinPath[i-1]]){
					p->weight+=ADD_Weight;
					break;
				}
				p=p->next;
			}
		}
		printf("\n");
		printf("cost:%d\n", Result);
	}
	else{
		PEdgeNode q;
		q=pGraph->vertexNode[S].firstEdge;
		int t=MAX_Weight,m=0;
		while(q!=NULL){
			if(q->ToVertex==MinPath[MinPathLength-1]&&t>q->weight){
				m=q->edgenum;
				t=q->weight;
			}
			q=q->next;
		}
		if(t>ADD_Weight&&t<2*ADD_Weight){
			Result-=ADD_Weight;
			same+=1;
		}
		printf("第二条路径edgenum:%d|", m);
		record_result(BACK_PATH,m);
		for(int i=MinPathLength-1,j=1;i>0;i--,j++){
			q=pGraph->vertexNode[MinPath[i]].firstEdge;
			t=MAX_Weight;
			m=0;
			while(q!=NULL){
				if(q->ToVertex==MinPath[i-1]&&t>q->weight){
					t=q->weight;
					m=q->edgenum;
				}
				q=q->next;
			}
			if(t>ADD_Weight&&t<2*ADD_Weight){
				Result-=ADD_Weight;
				same+=1;
			}
			printf("%d|", m);
			record_result(BACK_PATH,m);
		}
		printf("\n");
		printf("cost:%d\n", Result);
		printf("重边数:%d\n",same );
	}


	printf("定点序列Node:%d|", S);
	for(int i=MinPathLength-1;i>=0;i--)
		printf("%d|", MinPath[i]);
	printf("\n");
	
	
	return;
}
void TEST_printf(PGraphList pGraph)
{
    	//printf--topo-test
    	PEdgeNode p;
    	for(int i=0;i<20;i++){
		p=pGraph->vertexNode[i].firstEdge; 
		if(p!=NULL){
			printf("顶点：%d->",i);
			while(p!=NULL){
		        	printf("%d(%d)->",p->ToVertex,p->weight);  
		        	p=p->next;  
		   	}  
	   		printf("\n"); 
		} 
	}
	return ;
}
/**********************************以下为函数原型****************************************/
/***********该函数初始化图的基本参数***********/
bool InitGraph(PGraphList pGraph,char *topo[MAX_EDGE_NUM], int edge_num, char *demand[MAX_DEMAND_NUM])
{
	PEdgeNode p,pEdge;
	for(int i=0;i<2000;i++)
	{
		MyMinPath[i]=-1;
		AllUsedNode[i]=-1;

		AllNodes[i].Delete=true;//默认是删除的，如果读取数据时有这个点，则改为没被删除
		AllNodes[i].Demand=false;
		AllNodes[i].InDegree=0;
		AllNodes[i].OutDegree=0;
	
		for(int j=0;j<2000;j++)
		{
			Edge[1][i][j]=MAX_Weight;
			Edge[0][i][j]=-1;
		}

		pGraph->vertexNode[i].Delete=true;
		pGraph->vertexNode[i].Demand=false;
     	pGraph->vertexNode[i].vertex=-1;
     	pGraph->vertexNode[i].firstEdge=NULL;
	}

	int EdgeID=0,start=0,destination=0,weight=0;
	for(int i=0;i<edge_num;i++)		//读取Topo数据
	{
		if(GetTopoInfo(topo[i],EdgeID,start,destination,weight)==true)
		{
			if(Edge[0][start][destination]<0)//说明不是重边
			{

				if((AllNodes[start].InDegree==0)&&(AllNodes[start].OutDegree==0))
				{
					AllUsedNode[NodeNumber]=start;
					AllNodes[start].Delete=false;
					NodeNumber++;
				}
				if((AllNodes[destination].InDegree==0)&&(AllNodes[destination].OutDegree==0))
				{
					AllUsedNode[NodeNumber]=destination;
					AllNodes[destination].Delete=false;
					NodeNumber++;
				}

				Edge[0][start][destination]=EdgeID;
				Edge[1][start][destination]=weight;

				AllNodes[start].OutDegree=AllNodes[start].OutDegree+1;
				AllNodes[destination].InDegree=AllNodes[destination].InDegree+1;
			}
			else if(Edge[1][start][destination]>weight)
			{//如果是重边，则比较这两个权值，保留最小的
				Edge[0][start][destination]=EdgeID;
				Edge[1][start][destination]=weight;
			}
			/*******************************************************/
			//读取到start,destination改变顶点属性，使vertexNode[start].vertex=start表示顶点信息与数组中标号相同
			pGraph->vertexNode[start].Delete=false;
			pGraph->vertexNode[start].vertex=start;
			pGraph->vertexNode[destination].Delete=false;
			pGraph->vertexNode[destination].vertex=destination;

			pEdge=(PEdgeNode)malloc(sizeof(EdgeNode));  
	        	pEdge->next=NULL;
	        	pEdge->ToVertex=destination;
	  		pEdge->weight=weight;
	  		pEdge->edgenum=EdgeID;
  				
	       	p=pGraph->vertexNode[start].firstEdge; 
		    if(!p)//寻找链表尾指针
		    {
		        pGraph->vertexNode[start].firstEdge=pEdge;
		    }
		    else
		    {  
		        	while(p->next){
		            	p=p->next;
			     }
		        	p->next=pEdge;	  
		    }
			
		}
		else
			printf("Read Topo error!\n");
	}


	if(GetDemandInfo(demand[0])==false){
		return false;
	}


	return true;
}
/***********该函数用于读取Topo数据***********/
//num用来存放读一位数字之后目前读取的数是多少
//temp存储这次读取的这一位数字是什么
//place存储目前已经读取了几个信息,总共包括EdgeID,s,d,weight四个信息
bool GetTopoInfo(char *info,int &EdgeID,int &s,int &d,int &weight)
{
	int num=0,temp=0,place=1;
	bool start=false;
	for(int j=0;info[j]!='\n';j++)
	{
		if((info[j]>='0')&&(info[j]<='9'))
		{
			temp=info[j]-'0';
			num=num*10+temp;
			if((info[j+1]>='0')&&(info[j+1]<='9'))
				continue;
		}

		if((info[j+1]==',')||(info[j+1]=='|')||(info[j+1]=='\n'))//',' '|' '\n'均为数字分隔符
		{
			start=true;
		}

		if(start==true)
		{
			switch(place)
			{
				case 1:
					EdgeID=num;
					break;
				case 2:
					s=num;
					break;
				case 3:
					d=num;
					break;
				case 4:
					weight=num;
					return true;
				default:
					return false;
			}
			num=0;
			temp=0;
			place++;
			start=false;
		}	
	}
	return false;
}
/***********该函数用于读取Demand数据***********/
//place存储目前已经读取了几个信息,总共包括s,d,TargetNumber,TargetNodes[50]四个信息
bool GetDemandInfo(char *info)
{
	int i=0,num=0,temp=0,place=0;
	TargetNumber=0;
	bool start=false,end=false;
	for( i=0;info[i]!='\0';i++)
	{
		if((info[i]>='0')&&(info[i]<='9'))
		{
			start=true;
			end=false;
			temp=info[i]-'0';
			num=num*10+temp;
		}
		else 
		{
			if((start==true)&&(end==false))
			{
				if(place==1)
					S=num;
				if(place==2)
					D=num;
				if(place<(103)&&place>2)
				{
					if(AllNodes[num].Delete==true)
						return false;
					AllNodes[num].Demand=true;
					TargetNodes[place-3]=num;	
					TargetNumber++;
				}
				place++;
				start=false;
				end=true;
			}
			num=0;
			temp=0;
		}
	}
	if((info[i]=='\0')&&(start==true))
	{
		if(place==2)
			D=num;
		else
		{
			//如果必经点中存在图中未出现的点，则无解
			if(AllNodes[num].Delete==true)
				return false;

			AllNodes[num].Demand=true;
			TargetNodes[place-3]=num;	
			TargetNumber++;
			return true;
		}
	}
	return true;
}
//Dijkstra算法，判断从源点s，到终点d的最短路径是否存在,如果不存在返回false，
//如果存在，但题目中的源点S到目前源点s的距离length加上找到的最短路径大于已经找到的最优解Result的长度，
//则说明此路不可行，返回false
//LastDistance[602]存储最后必经点的个数为0时的路径信息，
//LastDistance[0]表示最短距离的长度，
//LastDistance[1]表示总共经过多少个顶点，之后的就是所经过的顶点序列,但LastDistance[MAX_NodeNumber+2]中的路径信息是逆序的
bool Dijkstra(PGraphList pGraph,int &MyNodeNumber,Node* MyAllNodes,int s,int d,int length,int* LastDistance)
{
	int edge[2000];
	bool find[2000];
	int parent[2000];
	int minLength=MAX_Weight;
	int minNode=-1;
	PEdgeNode p,q;
	q=pGraph->vertexNode[s].firstEdge;

	for(int i=0;i<NodeNumber;i++)
	{
		edge[AllUsedNode[i]]=MAX_Weight;
		if(AllUsedNode[i]==s)
		{
			find[AllUsedNode[i]]=true;
		}
		else if(MyAllNodes[AllUsedNode[i]].Delete==false)
		{
			find[AllUsedNode[i]]=false;
			parent[AllUsedNode[i]]=-1;
		}
	}
	while(q!=NULL){
		if(edge[q->ToVertex]>q->weight)
			edge[q->ToVertex]=q->weight;
		q=q->next;
	}

	int counter=0;//i统计已找到最短路径的定点个数，counter统计是当前图中顶点的个数
	for(int i=0;i<MyNodeNumber-1;i++)
	{
		minLength=MAX_Weight;
		minNode=-1;

		counter=0;
		for(int j=0;(j<NodeNumber)&&(counter<MyNodeNumber);j++)
		{
			if(MyAllNodes[AllUsedNode[j]].Delete==false)
			{	
				counter++;	
				if(find[AllUsedNode[j]]==false)
				{
					if(edge[AllUsedNode[j]]<=minLength)
					{
						minLength=edge[AllUsedNode[j]];
						minNode=j;
					}
				}
			}

		}
		if(minNode==-1)
			break;
		if(AllUsedNode[minNode]==d)
			break;
		find[AllUsedNode[minNode]]=true;
		if(minLength>=MAX_Weight)
		{
			if((MyAllNodes[AllUsedNode[minNode]].Demand==true)||(AllUsedNode[minNode]==d))
				return false;
			DeleteNode( AllUsedNode[minNode],MyNodeNumber,MyAllNodes);
		}

		p=pGraph->vertexNode[AllUsedNode[minNode]].firstEdge;
		while(p!=NULL){
			if(pGraph->vertexNode[p->ToVertex].Delete==false&&find[p->ToVertex]==false){
				if(edge[p->ToVertex]>minLength+p->weight){
					edge[p->ToVertex]=minLength+p->weight;
					parent[p->ToVertex]=AllUsedNode[minNode];
				}
			}
			p=p->next;
		}
	}


	if((edge[d]==MAX_Weight)||(edge[d]+length>Result))
	{
		return false;
	}

	LastDistance[0]=edge[d];
	LastDistance[1]=0;


	int last=d;
	for(int i=0;(i<MyNodeNumber-1)&&(last!=-1);i++)
	{


		LastDistance[2+LastDistance[1]]=last;
		last=parent[last];
		LastDistance[1]=LastDistance[1]+1;
	}
	LastDistance[2+LastDistance[1]]=s;
	LastDistance[1]=LastDistance[1]+1;

	return true;
}


bool MinDistance(PGraphList pGraph,Node* NewAllNodes,int NewNodeNumber,int NewTargetNumber,int s,int d,int length,int distance[100][2002])
{
	//NeedNodeNumber存储未找到最短距离的必经节点的个数
	int NeedNodeNumber=NewTargetNumber;
	int edge[2000];
	bool find[2000];
	int parent[2000];
	int minLength=MAX_Weight;
	int minNode=-1;
	bool ReachEnd=false;//用来返回，true代表s可以不经过必经点到达终点
	PEdgeNode p,q;
	q=pGraph->vertexNode[s].firstEdge;

	for(int i=0;i<NodeNumber;i++)
	{
		edge[AllUsedNode[i]]=MAX_Weight;
		if(AllUsedNode[i]==s)
		{
			find[AllUsedNode[i]]=true;
		}
		else if(NewAllNodes[AllUsedNode[i]].Delete==false)
		{
			find[AllUsedNode[i]]=false;
			parent[AllUsedNode[i]]=-1;
		}

	}
	while(q!=NULL){
		if(edge[q->ToVertex]>q->weight)
			edge[q->ToVertex]=q->weight;
		q=q->next;
	}

	int counter=0;
	for(int i=0;(i<NewNodeNumber-1)&&(NeedNodeNumber>0);i++)
	{
		minLength=MAX_Weight;
		minNode=-1;

		counter=0;//i统计已找当前图中顶点的个数
		for(int j=0;(j<NodeNumber)&&(counter<NewNodeNumber);j++)
		{
			if(NewAllNodes[AllUsedNode[j]].Delete==false)
			{	
				counter++;

				if(find[AllUsedNode[j]]==false)
				{
					if(edge[AllUsedNode[j]]<=minLength)
					{
						minLength=edge[AllUsedNode[j]];
						minNode=j;
					}
				}
			}
		}
		if(minNode==-1)
			break;
		find[AllUsedNode[minNode]]=true;


		if(NewAllNodes[AllUsedNode[minNode]].Demand==true)
			NeedNodeNumber--;
		else if(AllUsedNode[minNode]!=d)
		{
			p=pGraph->vertexNode[AllUsedNode[minNode]].firstEdge;
			while(p!=NULL){
				if(pGraph->vertexNode[p->ToVertex].Delete==false&&find[p->ToVertex]==false){
					if(edge[p->ToVertex]>minLength+p->weight){
						edge[p->ToVertex]=minLength+p->weight;
						parent[p->ToVertex]=AllUsedNode[minNode];
					}
				}
				p=p->next;
			}
		}
		else
		{
			ReachEnd=true;
		}


	}
 //printf("edge[2]:%d\n",edge[2]);

	for(int j=0;j<TargetNumber;j++)
	{
		if((NewAllNodes[TargetNodes[j]].Delete==false)&&(NewAllNodes[TargetNodes[j]].Demand==true))
		{
			if(edge[TargetNodes[j]]<MAX_Weight)
			{
				distance[j][0]=edge[TargetNodes[j]];
				distance[j][1]=0;
				int last=TargetNodes[j];

				for(int i=0;(i<NewNodeNumber-2)&&(last!=-1);i++)
				{
					distance[j][2+distance[j][1]]=last;
					last=parent[last];
					distance[j][1]=distance[j][1]+1;
				}

			}
			else
			{
				distance[j][0]=MAX_Weight;
				distance[j][1]=0;
			}
		}

	}

	return ReachEnd;
}

void SortDistance(Node *MyAllNodes,int MyNodeNumber,int distance[100][2002],int *Turn)
{
	int temp=0,counter=0;

	for(int i=0;i<TargetNumber;i++)
		if((MyAllNodes[TargetNodes[i]].Delete==false)&&(MyAllNodes[TargetNodes[i]].Demand==true))
		{
			Turn[counter]=i;
			counter++;
		}

		for(int i=1;i<counter;i++)
		{
			for(int j=i;j>0;j--)
			{
				if(distance[Turn[j]][0]<distance[Turn[j-1]][0])
				{
					temp=Turn[j];
					Turn[j]=Turn[j-1];
					Turn[j-1]=temp;
				}
				else
					break;
			}
		}
}
/***********该函数用于***********/
//deep是递归的深度,length是从题目源点S到现在源点s的距离,
//如果中间必经顶点的个数MyTargetNumber为0，则直接求现在源点s到终点D的距离即可。


bool MySearch(int MyNodeNumber,Node* MyAllNodes,int MyTargetNumber,int s,int d,int deep,int length)
{
	if(MyTimeOut())
		return true;
	if(deep==1)
		gettimeofday(&return_start, NULL);
	if(Distory_TimeOut()){
		Distory=1;
		return true;
	}
	int LastDistance[2002];

	if(DeleteUseless(MyAllNodes,MyNodeNumber,s,d)==false){
		//printf("deep:%d\n",deep );
		return false;
	}
	if(Dijkstra(&Graph,MyNodeNumber, MyAllNodes,s, d,length,LastDistance)==false){
		//printf("deep:%d\n",deep );
		return false;
	}
	if(MyTargetNumber==0)
	{
		gettimeofday(&return_start, NULL);
		printf("得到一个最优解%d,%d\n",length+LastDistance[0],Result);
		if(length+LastDistance[0]<Result)
		{
			Result=length+LastDistance[0];
			SetMinPath(LastDistance);
		}
		return true;
	}
	//求出源点到所有必经点的距离(这个路径上不能包含必经点和起点、终点)distance[50][602]；
	//其中distance[i]为s到i的最短路径信息，
	//distance[i][0]表示最短距离的长度，
	//distance[i][1]表示总共经过多少个顶点，之后的就是所经过的顶点序列
	MinDistance(&Graph,MyAllNodes,MyNodeNumber,MyTargetNumber,s,d,length,StoreDistances[deep]);

	//对distance[50][602]进行排序，按照从小到大的顺序，把对应的顶点坐标存在数组Turn[50]中
	int Turn[100];
	SortDistance(MyAllNodes,MyNodeNumber,StoreDistances[deep],Turn);

	//如果目前的长度length加上该最短距离大于已得到的最优解Result的值，则当前源点s不存在最优解,返回false；
	//否则，删除源点并以最近的点为源点继续递归执行
	Node NewAllNodes[2000];
	for(int i=0; i<MyTargetNumber; i++)
	{
		if(MyTimeOut())
			return true;
		if(Distory_TimeOut()){
			Distory=1;
			return true;
		}
		if(Turn[i]>100)
			return false;
		if((StoreDistances[deep][Turn[i]][0]+length>=Result))
			return false;

		//复制一个原图的副本，以后均操作副本
		int NewNodeNumber=MyNodeNumber;
		int NewTargetNumber=MyTargetNumber;
		CopyArray(MyAllNodes,MyNodeNumber,NewAllNodes);	

		AddMyMinPath(MyMinPath,StoreDistances[deep],s,Turn[i]);

		//删除源点s，新的原点变为TargetNodes[Turn[i]]，DeleteSourceNode的参数只需要Turn[i]
		DeleteSourceNode(s,NewNodeNumber,NewAllNodes,NewTargetNumber,StoreDistances[deep],Turn[i]);
		MySearch(NewNodeNumber,NewAllNodes,NewTargetNumber,TargetNodes[Turn[i]],D,deep+1,length+StoreDistances[deep][Turn[i]][0]);
		if(Distory==1&&deep!=1){
			return true;
		}
		else if(Distory==1&&deep==1){
			//printf("Q\n");
			gettimeofday(&return_start, NULL);
			Distory=0;
		}
	}
	return true;
}

//把最近的一条路径加入当前最优路径信息中,index是distance的索引
void AddMyMinPath(int *MyMinPath,int distance[100][2002],int s,int index)
{
	int i;
	//distance中存的路径上的顶点是逆序的,所以要逆序遍历
	for( i=2;i<1+distance[index][1];i++)
	{
		MyMinPath[distance[index][i]]=distance[index][i+1];
	}
	MyMinPath[distance[index][i]]=s;
}

//计算出最优路径信
void SetMinPath(int *LastDistance)
{
	int i=0;
	MinPathLength=0;
	//distance中存的路径上的顶点是逆序的,所以要逆序遍历
	for(i=2;i<1+LastDistance[1];i++)
	{
		MyMinPath[LastDistance[i]]=LastDistance[i+1];
	}

	for(i=D;(i<2000)&&(MyMinPath[i]!=-1);i=MyMinPath[i])
	{
		MinPath[MinPathLength]=i;
		MinPathLength++;
	}		

}
//s是当前源点，NextSource是下一个源点，distance[50][602]表示s到所有下一源点TargetNodes[NextSource]的最短路径信息,NextSource其实是下一个原点在TargetNodes[]中的序号
void DeleteSourceNode(int s,int &NewNodeNumber,Node* NewAllNodes,int &NewTargetNumber,int distance[100][2002],int NextSource)
{
	DeleteNode( s,NewNodeNumber,NewAllNodes);
	//由于s第一次是S，所以之后每次都不把原点当作必经点，所以必经点的个数只需要在TargetNodes[NextSource]时减1即可
	//把下一个原点设为起点，不需要删除，只需要NewTargetNumber减1
	NewAllNodes[TargetNodes[NextSource]].Demand=false;
	NewTargetNumber=NewTargetNumber-1;
	//distance中存的路径上的顶点是逆序的,所以要逆序遍历，并且起点不需要删除,所以i>1,这些点都为非必经点，所以NewTargetNumber不需要减1
	for(int i=distance[NextSource][1];i>1;i--)
		DeleteNode(distance[NextSource][1+i],NewNodeNumber,NewAllNodes);
}

//复制图的基本信息,由于在遍历的过程中，边的序号不会被改变，所以只需要边的权值，用MyEdge[600][600]代表Edge[1][600][600]
//复制数组的大小是NodeNumber，不是MyNodeNumber
void CopyArray(Node* MyAllNodes,int MyNodeNumber,Node *NewAllNodes)
{
	for(int i=0; i<NodeNumber; i++)
	{
		NewAllNodes[AllUsedNode[i]].InDegree=MyAllNodes[AllUsedNode[i]].InDegree;
		NewAllNodes[AllUsedNode[i]].OutDegree=MyAllNodes[AllUsedNode[i]].OutDegree;
		NewAllNodes[AllUsedNode[i]].Delete=MyAllNodes[AllUsedNode[i]].Delete;
		NewAllNodes[AllUsedNode[i]].Demand=MyAllNodes[AllUsedNode[i]].Demand;
	}
}

//简化副本图,简化时如果出现必经顶点的出度或者入度为0 则返回false,
//简化的时候顶点的个数可能会改变，但必经顶点的个数不需要修改，因为如果有必经顶点，则无解
bool DeleteUseless(Node* NewAllNodes,int &NewNodeNumber,int s,int d)
{
	bool Change=true;
	for(;Change==true;)
	{
		Change=false;//每次都假设没改变，如果有节点被删除，则会继续循环，直到没有节点被修改为止
		if(NewAllNodes[s].OutDegree==0||NewAllNodes[d].InDegree==0)
		{
			//printf("11\n");
			return false;
		}

		for(int i=0;i<NodeNumber;i++)
		{
			if(NewAllNodes[AllUsedNode[i]].Delete==false)
			{
				if(AllUsedNode[i]==s)
				{
					if(NewAllNodes[AllUsedNode[i]].OutDegree==0)
					{
						//printf("1111\n");
						return false;
					}
				}
				else if(AllUsedNode[i]==d)
				{
					if(NewAllNodes[AllUsedNode[i]].InDegree==0)
					{
						//printf("111\n");
						return false;
					}
				}
				else if(NewAllNodes[AllUsedNode[i]].InDegree==0||NewAllNodes[AllUsedNode[i]].OutDegree==0)
				{
					Change=true;
					if((NewAllNodes[AllUsedNode[i]].Demand==true)){
						//printf("33AllUsedNode[i]:%d\n",AllUsedNode[i]);
						return false;
					}
					else
					{
						DeleteNode(AllUsedNode[i],NewNodeNumber, NewAllNodes);
					}

				}
			}
		}
	}
	return true;
}

//i是要被删掉的顶点,删除顶点除了要修改出度和入度，也要修改顶点的个数，
//但不需要修改顶点之间的权值(其实不用修改权值，求最短路径的时候被删掉的顶点不会被访问)
void DeleteNode(int DeleteNodeNumber,int &NewNodeNumber,Node* NewAllNodes)
{
	if(NewAllNodes[DeleteNodeNumber].Delete==true)
		return;

	NewAllNodes[DeleteNodeNumber].Delete=true;
	NewNodeNumber=NewNodeNumber-1;
	for(int i=0;i<NodeNumber;i++){
		if(NewAllNodes[AllUsedNode[i]].Delete==false&&AllUsedNode[i]!=DeleteNodeNumber)
		{
			if(Edge[1][DeleteNodeNumber][AllUsedNode[i]]!=MAX_Weight)
				NewAllNodes[AllUsedNode[i]].InDegree=NewAllNodes[AllUsedNode[i]].InDegree-1;
			if(Edge[1][AllUsedNode[i]][DeleteNodeNumber]!=MAX_Weight)
				NewAllNodes[AllUsedNode[i]].OutDegree=NewAllNodes[AllUsedNode[i]].OutDegree-1;
		}

	}
}
void ADD_MU(PGraphList pGraph)
{
    	PEdgeNode p;
    	for(int i=0;i<2000;i++){
		p=pGraph->vertexNode[i].firstEdge; 
		if(p!=NULL){
			while(p!=NULL){
				if(p->weight<50)
					p->weight=100-p->weight;
				else if(p->weight>ADD_Weight&&p->weight<(ADD_Weight+50))
					p->weight=2*ADD_Weight+50-p->weight;
				/*if(p->weight>ADD_Weight&&p->weight<2*ADD_Weight)
					p->weight=2*ADD_Weight+100-p->weight;
				else
		        		p->weight=100-p->weight;*/
		        	p=p->next;  
		   	}  
		} 
	}
	return ;
}