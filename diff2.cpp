//问题：比如：dd,123456dd,,,,d,123456d
//办法：每次自动检查相同的前缀和后缀

#include <iostream>
#include <string>
#include <list>
#include <ctime>

//#define _DEBUG_

using namespace std;

enum Operation
{
	EQUAL, DELETE, INSERT
};

class Diff
{
public:
	Operation operation;
	string text;
	Diff(){}
	Diff(Operation _operation, string _text)
	{
		operation = _operation;
		text = _text;
	}
};

list<Diff*> diffs;

void cmp_files(string text1, int N, string text2, int M,int updown)
{
	//cout<<text1<<">>>>1>>>>"<<text2<<endl;
	bool flag = false;
	string push_end="";
	if(N!=0&&M!=0)
	{
		int i=0,j;
		
		while(i<N&&i<M && text1[i]==text2[i])//去掉共同前缀
		{
			i++;
			flag = true;
		}
		if(flag)
		{
			diffs.push_back(new Diff(EQUAL,text1.substr(0,i)));
		
			text1=text1.substr(i);
			text2=text2.substr(i);//前后缀没有加到diffs
			N=text1.size();
			M=text2.size();
			//cout<<text1<<">>>>2>>>>"<<text2<<endl;
		}
		


		 i=N;j=M;
		 flag = false;
		while(i>0&&j>0 && text1[i-1]==text2[j-1])//去掉共同后缀
		{
			//cout<<text1[i-1]<<"=="<<text2[j-1]<<endl;
			i--;j--;
			flag=true;
		}
		//cout<<i<<" "<<j<<endl;
		
		
		if(flag)
		{
			push_end = text1.substr(i);
			//cout<<push_end<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl;
		
			text1=text1.substr(0,i);
			text2=text2.substr(0,j);
			N=text1.size();
			M=text2.size();
		}
	}
	//cout<<text1<<">>>>>3>>>"<<text2<<endl;
	//cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
	if(M==0 && N>0)
	{
		diffs.push_back(new Diff(DELETE, text1));
		//cout<<"--------->>>>>>"<<text1<<endl;
		if(flag)//push_end???????????????????????????????? 后缀加在这个地方才可以显示在正确的位置
		{
			diffs.push_back(new Diff(EQUAL,push_end));
		}
		return;
	}
	if(N==0 && M>0)
	{
		diffs.push_back(new Diff(INSERT, text2));
		//cout<<"+++++++>>>>>>>>>"<<text2<<endl;
		if(flag)//push_end???????????????????????????????? 后缀加在这个地方才可以显示在正确的位置
		{
			diffs.push_back(new Diff(EQUAL,push_end));
		}
		return;
	}
	if(N<=0 && M<=0)
	{
		if(flag)//push_end???????????????????????????????? 后缀加在这个地方才可以显示在正确的位置
		{
			diffs.push_back(new Diff(EQUAL,push_end));
		}
		return ;
	}
		

	

#ifdef _DEBUG_
	cout<<"---------"<<updown<<"---------"<<endl;
	cout<<"text1:-"<<text1<<"-"<<endl;
	cout<<"text2:-"<<text2<<"-"<<endl;
#endif
	int delta = N - M;
	int max_d = (N + M + 1) /2;

	int *V1;
	int *V2;
	int base = max_d;

	V1 = new int[4*max_d+1];
	V2 = new int[4*max_d+1];

	for(int i=0;i<=4*max_d;i++)//初始化之后delete就不报错了，想不通。。。。。
	{
		V1[i]=-9999999;
		V2[i]=-9999999;
	}

  	V1[base+1]=0;
  	V2[base+1]=0;
	for(int d=0; d<=max_d; d++)
	{
		for(int k=-d; k<=d; k+=2)
		{
			bool down = ( k == -d || ( k != d && V1[base+ k - 1 ] < V1[base+ k + 1 ] ) );

			int kPrev = down ? k + 1 : k - 1;

			// start point
			int xStart = V1[base+ kPrev];
			int yStart = xStart - kPrev;

			// mid point
			int xMid = down ? xStart : xStart + 1;
			int yMid = xMid - k;

			// end point
			int xEnd = xMid;
			int yEnd = yMid;

			// follow diagonal

			while ( xEnd < N && yEnd < M && text1[ xEnd ] == text2[ yEnd ] )
			{ xEnd++; yEnd++; }
#ifdef _DEBUG_
			cout<<d<<" odd "<<xEnd<<":"<<yEnd<<endl;
#endif

			V1[base+ k ] = xEnd;
#ifdef _DEBUG_
			cout<<"V1["<<k<<"]="<<xEnd<<endl;
#endif

			if((delta%2)!=0 && (k>= delta - (d-1) && k <= delta + (d-1)))
			{
				
				int r_xEnd = N - V2[base + delta-k];
				if(xEnd >= r_xEnd)
				{
					delete [] V1;
					delete [] V2;
					int D = 2*d -1;
#ifdef _DEBUG_
					cout<<"o********"<<D<<"  "<<((M+N)-D)/2<<endl;
					cout<<xStart<<" "<<yStart<<" "<<xMid<<" "<<yMid<<" "<<xEnd<<" "<<yEnd<<endl;
#endif
					if(D >= 1)
					{
						string text1a = text1.substr(0,xStart);
						string text1b = "";
						if(xEnd<N)
							text1b= text1.substr(xEnd);
						string text2a = text2.substr(0,yStart);
						
						string text2b="";
						if(yEnd<M)// 是M>>>>>>>>>>>>>>>>>>
							text2b= text2.substr(yEnd);
#ifdef _DEBUG_						
						cout<<"t1a:"<<text1a<<"-"<<endl;
						cout<<"t1b:"<<text1b<<"-"<<endl;
						cout<<"t2a:"<<text2a<<"-"<<endl;
						cout<<"t2b:"<<text2b<<"-"<<endl;
#endif
						
						cmp_files(text1a,text1a.size(),text2a,text2a.size(),1);

						
						if(d != 0 &&yMid == yStart )//move right -->
						{
							diffs.push_back(new Diff(DELETE,text1.substr(xMid-1,1)));
							cout<<"---------------"<<text1[xMid-1]<<"---------"<<endl;
						}
						if(d != 0 && xMid == xStart)//move down
						{
							diffs.push_back(new Diff(INSERT,text2.substr(yMid-1,1)));
							cout<<"+++++++++++++++"<<text2[yMid-1]<<"+++++++++++"<<endl;
						}
						if(!text1.substr(xMid,xEnd-xMid).empty())// odd 部分，EQUAL 在DELETE,INSERT之后，而even部分在之前
							diffs.push_back(new Diff(EQUAL, text1.substr(xMid,xEnd-xMid)));
						
#ifdef _DEBUG_
						cout<<"D>1============="<<text1.substr(xMid,xEnd-xMid)<<"============"<<endl;
#endif
						//diffs.push_back(new Diff(INSERT, text2));
						cmp_files(text1b,text1b.size(),text2b,text2b.size(),2);
					}

					else if(D ==0)
					{
						diffs.push_back(new Diff(EQUAL, text1));
						//text2=text1="";
#ifdef _DEBUG_
						cout<<"D=0============="<<text1<<"=========="<<endl;
#endif
					}
					if(flag)//push_end???????????????????????????????? 后缀加在这个地方才可以显示在正确的位置
					{
						diffs.push_back(new Diff(EQUAL,push_end));
					}
					return;
					//***********************************
				}
			}

		}

		for(int k=-d; k<=d; k+=2)
		{
			bool down = ( k == -d || ( k != d && V2[base+ k - 1 ] < V2[base+ k + 1 ] ) );

			int kPrev = down ? k + 1 : k - 1;

			// start point
			int xStart = V2[base+ kPrev];
			int yStart = xStart - kPrev;

			// mid point
			int xMid = down ? xStart : xStart + 1;
			int yMid = xMid - k;

			// end point
			int xEnd = xMid;
			int yEnd = yMid;

			// follow diagonal

			while ( xEnd < N && yEnd < M && text1[N - xEnd -1] == text2[M - yEnd -1] )
			{ xEnd++; yEnd++; }
#ifdef _DEBUG_
			cout<<d<<" eve "<<xEnd<<":"<<yEnd<<endl;
#endif
			V2[base+ k] = xEnd;
#ifdef _DEBUG_
			cout<<"V2["<<k<<"]="<<xEnd<<endl;
#endif

			if((delta%2)==0 && (k>= -d && k <= d ))//去掉了delta
			{
				
				int r_xEnd = V1[base + delta-k];
#ifdef _DEBUG_
				cout<<delta<<"-"<<k<<"="<<delta-k<<" == "<<N-xEnd<<"<="<<r_xEnd<<endl;
#endif
				if(N-xEnd <= r_xEnd)//<=???????
				{
#ifdef _DEBUG_
					cout<<"here even"<<endl;
					cout<<V1[0]<<"+++"<<endl;
#endif
					delete [] V1;
					delete [] V2;
					int D = 2*d;
#ifdef _DEBUG_
					cout<<"e******"<<D<<" "<<((M+N)-D)/2<<endl;
#endif
					if(D >= 1)
					{	
						xStart = N - xStart;
						yStart = M - yStart;
						xEnd = N - xEnd;
						yEnd = M -yEnd;
						xMid = N -xMid;
						yMid = M -yMid;
#ifdef _DEBUG_
						cout<<xStart<<" "<<yStart<<" "<<xMid<<" "<<yMid<<" "<<xEnd<<" "<<yEnd<<endl;
#endif
						string text1a = text1.substr(0,xEnd);
						string text1b = text1.substr(xStart);
						string text2a = text2.substr(0,yEnd);
						string text2b = text2.substr(yStart);
#ifdef _DEBUG_
						cout<<"t1a:"<<text1a<<"-"<<endl;
						cout<<"t1b:"<<text1b<<"-"<<endl;
						cout<<"t2a:"<<text2a<<"-"<<endl;
						cout<<"t2b:"<<text2b<<"-"<<endl;
#endif

						
						cmp_files(text1a,text1a.size(),text2a,text2a.size(),1);
						//cout<<"here1"<<endl;

						string temp = text1.substr(xEnd,xMid-xEnd);
						if(!temp.empty())
							diffs.push_back(new Diff(EQUAL, temp));


						if(d != 0 && yMid == yStart)//move left,d==0 locate at(0,0)
						{
							diffs.push_back(new Diff(DELETE,text1.substr(xStart-1,1)));
							cout<<"-------------"<<text1[xStart-1]<<"-----------"<<endl;
						}
						if(d != 0 && xMid == xStart)//move up
						{
							diffs.push_back(new Diff(INSERT,text2.substr(yStart-1,1)));
							cout<<"++++++++++++"<<text2[yStart-1]<<"++++++++++++++"<<endl;
						}
						
#ifdef _DEBUG_
						cout<<"D>1============"<<temp<<"============="<<endl;
#endif
						//diffs.push_back(new Diff(INSERT, text2));
						cmp_files(text1b,text1b.size(),text2b,text2b.size(),2);
						//cout<<"here2"<<endl;
					}
					else if(D ==0)
					{
						if(!text1.empty())
						{
							diffs.push_back(new Diff(EQUAL, text1));
							//text2=text1="";
#ifdef _DEBUG_
							cout<<"D=0++++++++++++="<<text1<<"=++++++++++++"<<endl;
#endif
						}
						//cout<<"here3"<<endl;
					}
					//cout<<"here4"<<endl;
					if(flag)//push_end???????????????????????????????? 后缀加在这个地方才可以显示在正确的位置
					{
						diffs.push_back(new Diff(EQUAL,push_end));
					}
					return;
					//***********************************
				}
			}
		}
	}

}

//for debug
int main()
{
    string a,b;
	freopen("in.txt","r",stdin);
	cin>>a;
	freopen("in2.txt","r",stdin);
    cin>>b;
	cout<<a.size()<<"     "<<b.size()<<endl;
	clock_t begin,end;
	begin = clock();
	cmp_files(a,a.size(),b,b.size(),0);
	end = clock();
	cout<<end-begin<<endl;

	list<Diff*>::iterator it = diffs.begin();
	cout<<"--------------------"<<endl;
	for(;it != diffs.end(); it++)
	{
		if((*it)->operation == INSERT)
			cout<<"+";
		else if((*it)->operation == DELETE)
			cout<<"-";
		else 
			cout<<"=";
		cout<<(*it)->text<<endl;
	}

	cout<<"\n--------------------"<<endl;
	return 0;
}


