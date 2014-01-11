//改进地方: 1.当访问到边界时，k的值范围也可以缩小了,见neil的算法实现
//         2.++++----+++不能顺序的先+后-
//		   3.空行也匹配了，那么就出现了问题，可能空行很多，导致优先匹配了空行而忽略掉非空行的匹配
//         4.结果空行影响输出美观直观，++-- --++中间的空行却隔开了联系在一起的东西，对结果处理空行

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <ctime>
#include <list>
#include <map>

using namespace std;


wstring file1;
wstring file2;
map<string,int> mapStringToInt;
vector<string> intToString;


enum Operation
{
	EQUAL, DELETE, INSERT
};

class Diff
{
public:
	Operation operation;
	wstring text;
	Diff(){}
	Diff(Operation _operation, wstring _text)
	{
		operation = _operation;
		text = _text;
	}
};

list<Diff*> diffs;

void cmp_files(wstring text1, int N, wstring text2, int M)
{
	bool hasCommSuffix = false;
	wstring push_end;
	if(N!=0&&M!=0)
	{
		int i=0,j;
		
		while(i<N&&i<M && text1[i]==text2[i])//去掉共同前缀    
		{
			i++;
			hasCommSuffix = true;
		}
		if(hasCommSuffix)
		{
			diffs.push_back(new Diff(EQUAL,text1.substr(0,i)));
		
			text1=text1.substr(i);
			text2=text2.substr(i);
			N=text1.size();
			M=text2.size();
		}
		
		i=N;j=M;
		hasCommSuffix = false;
		while(i>0&&j>0 && text1[i-1]==text2[j-1])//去掉共同后缀    
		{
			i--;j--;
			hasCommSuffix=true;
		}
		
		if(hasCommSuffix)
		{
			push_end = text1.substr(i);
			text1=text1.substr(0,i);
			text2=text2.substr(0,j);
			N=text1.size();
			M=text2.size();
		}
	}
	if(M==0 && N>0)
	{
		diffs.push_back(new Diff(DELETE, text1));
		if(hasCommSuffix)
		{
			diffs.push_back(new Diff(EQUAL,push_end));
		}
		return;
	}
	if(N==0 && M>0)
	{
		diffs.push_back(new Diff(INSERT, text2));
		if(hasCommSuffix)
		{
			diffs.push_back(new Diff(EQUAL,push_end));
		}
		return;
	}
	if(N<=0 && M<=0)
	{
		if(hasCommSuffix)
		{
			diffs.push_back(new Diff(EQUAL,push_end));
		}
		return ;
	}

	int delta = N - M;
	int max_d = (N + M + 1) /2;

	int *V1;
	int *V2;
	int base = 2*max_d;//??????????????忘记*2，悲剧了

	V1 = new int[4*max_d+1];//4???????????????????????????????????????????
	V2 = new int[4*max_d+1];

	for(int i=0;i<=4*max_d;i++)
	{
		V1[i] = -99999;//numeric_limits<int>::min();
		V2[i] = -99999;//numeric_limits<int>::min();
	}

  	V1[base+1]=0;
  	V2[base+1]=0;
	int k1start = 0;
	int k1end = 0;
	int k2start = 0;
	int k2end =0;

	for(int d=0; d<=max_d; d++)
	{
		for(int k=-d+k1start; k<=d-k1end; k+=2)
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

			V1[base+ k ] = xEnd;
			//去掉这个。。。。。。。。。。。。。。。就可以了。。。。。。。为什么啊
			/*if(xEnd > N)
			{
			k1end+=2;
			}
			else if(yEnd > M)
			{
			k1start+=2;
			}*/
			//cout<<"V1["<<k<<"]="<<xEnd<<endl;
			//delta其实就是反方向的k=0的在正方向看来的k值
		    if((delta%2)!=0 && (k>= delta - (d-1) && k <= delta + (d-1)) && V2[base + delta-k]!=-99999)//去掉了后面这个限制条件就会出错，为什么？？？？？
			{
				int r_xEnd = N - V2[base + delta-k];
				//cout<<"v2[base+delata-k]="<<V2[base + delta-k]<<endl;
				//cout<<"1-->>"<<delta<<"-"<<k<<"="<<delta-k<<" == "<<xEnd<<"<="<<r_xEnd<<endl;
				if(xEnd >= r_xEnd)
				{
					delete [] V1;
					delete [] V2;
					int D = 2*d -1;

					if(D >= 1)
					{
						//cout<<xStart<<" "<<yStart<<" "<<xMid<<" "<<yMid<<" "<<xEnd<<" "<<yEnd<<endl;
						wstring text1a = text1.substr(0,xStart);
						wstring text1b;
						if(xEnd<N)
							text1b= text1.substr(xEnd);
						wstring text2a = text2.substr(0,yStart);

						wstring text2b;
						if(yEnd<M)
							text2b= text2.substr(yEnd);
	
						cmp_files(text1a,text1a.size(),text2a,text2a.size());
						
						if(d != 0 &&yMid == yStart )//move right
						{
							diffs.push_back(new Diff(DELETE,text1.substr(xMid-1,1)));
						}
						//加一个else是否更好？
						if(d != 0 && xMid == xStart)//move down
						{
							diffs.push_back(new Diff(INSERT,text2.substr(yMid-1,1)));
						}
						if(!text1.substr(xMid,xEnd-xMid).empty())// odd 部分，EQUAL 在DELETE,INSERT之后，而even部分在之前
							diffs.push_back(new Diff(EQUAL, text1.substr(xMid,xEnd-xMid)));

						cmp_files(text1b,text1b.size(),text2b,text2b.size());
					}

					else if(D ==0)
					{
						diffs.push_back(new Diff(EQUAL, text1));
		
					}
					if(hasCommSuffix)
					{
						diffs.push_back(new Diff(EQUAL,push_end));
					}
					return;
				}
			}

		}

		for(int k=-d+k2start; k<=d-k2start; k+=2)
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

			V2[base+ k] = xEnd;
			//if(xEnd>N)
			//{
			//	k2end+=2;
			//}
			//else if(yEnd>M)
			//{
			//	k2start+=2;
			//}
			////cout<<"V2["<<k<<"]="<<N-xEnd<<endl;
			//else 
			if((delta%2)==0 && V1[base + delta-k]!=-99999 &&(k>=delta-d&&k<=delta+d))//，这个地方应该加一个限制条件
			{

				int r_xEnd = V1[base + delta-k];
				//cout<<"v1[base+delata-k]="<<V1[base + delta-k]<<endl;
				//int r_xEnd = V1[base + delta-k];//问题应该出现在这个地方。。。。。。。。
				//cout<<"2-->>"<<delta<<"-"<<k<<"="<<delta-k<<" == "<<N-xEnd<<"<="<<r_xEnd<<endl;
				if(N-xEnd <= r_xEnd)//<=???????
				{
					delete [] V1;
					delete [] V2;
					int D = 2*d;

					if(D >= 1)
					{	
						xStart = N - xStart;
						yStart = M - yStart;
						xEnd = N - xEnd;
						yEnd = M -yEnd;
						xMid = N -xMid;
						yMid = M -yMid;
						//cout<<xStart<<" "<<yStart<<" "<<xMid<<" "<<yMid<<" "<<xEnd<<" "<<yEnd<<endl;
						wstring text1a = text1.substr(0,xEnd);
						wstring text1b = text1.substr(xStart);
						wstring text2a = text2.substr(0,yEnd);
						wstring text2b = text2.substr(yStart);
					
						cmp_files(text1a,text1a.size(),text2a,text2a.size());

						wstring temp = text1.substr(xEnd,xMid-xEnd);
						if(!temp.empty())
							diffs.push_back(new Diff(EQUAL, temp));
						

						if(d != 0 && xMid == xStart)//move up
						{
							diffs.push_back(new Diff(INSERT,text2.substr(yStart-1,1)));
						}
						
						if(d != 0 && yMid == yStart)//move left,d==0 locate at(0,0)
						{
							diffs.push_back(new Diff(DELETE,text1.substr(xStart-1,1)));
						}
						cmp_files(text1b,text1b.size(),text2b,text2b.size());
					}
					else if(D ==0)
					{
						if(!text1.empty())
						{
							diffs.push_back(new Diff(EQUAL, text1));
						}
					}
					if(hasCommSuffix)
					{
						diffs.push_back(new Diff(EQUAL,push_end));
					}
					return;
				}
			}
		}
	}

}

//能否用二进制读入，更快？有时间尝试一下
void FileRead(string FileName, wstring& file) 
{
	// 这个函数是基于流的方式,速度也比较快
	if (file.size() > 0)
	{
		file.clear();
	}
	ifstream fin(FileName.c_str());
	if (!fin.bad()) 
	{
		string sLine; // 行字符串
		while (getline(fin, sLine)) {
			if(!mapStringToInt.count(sLine))
			{
				mapStringToInt[sLine] = mapStringToInt.size();
				file+=(wchar_t(mapStringToInt.size()-1));//需不需要先转换程ushort?
				intToString.push_back(sLine);
			}
			else
			{
				file+=(wchar_t(mapStringToInt[sLine]));
			}
		}
	}
	fin.close();
}
//=======================================================================
//排序目的：使得+ + - + - + +这种情况变为- - + + + + +，即把 - 移动到 + 之前
//算法描述：找到第一个+，然后向后遍历遇到 + 则继续遍历，遇到 = 则重新找到之后
//          的 + 号，也就是重新开始，如果遇到 -号（当然是在遇到=之前），则把
//          该 - 的节点插入第一个+之前
//时间复杂度：O(N)
//std::list list.splice(it,list,it2),在it之前插入it2指向的元素，（相当于移位）it2还是指向插入的那个元素
//=======================================================================
void sortDiffs()
{
	list<Diff*>::iterator it;
	list<Diff*>::iterator it2;//+
	list<Diff*>::iterator it3;//-
	for(it = diffs.begin();it != diffs.end(); it++)
	{
		if((*it)->operation == INSERT)
		{
			it2 = it++;
			for(;it!=diffs.end();)
			{
				if((*it)->operation == EQUAL)
				{
					break;
				}
				else if((*it)->operation == INSERT)
				{
					it++;
					continue;
				}
				else
				{
					it3 = it;
					it++;
					diffs.splice(it2,diffs,it3);
				}
			}
		}
		if(it==diffs.end())
			break;
	}
	
}
 
//void postProcess()//处理空格     
//{
//	list<Diff*>::iterator it;
//	list<Diff*>::iterator it2;//+
//	list<Diff*>::iterator it3;//-
//	for(it = diffs.begin();it != diffs.end(); it++)
//	{
//		if((*it)->operation == INSERT || (*it)->operation == DELETE)
//		{
//			it2 = it++;
//			for(;it!=diffs.end();)
//			{
//				//intToString[int((*it)->text)];
//				if((*it)->operation == EQUAL)
//				{
//					break;
//				}
//				else if((*it)->operation == INSERT)
//				{
//					it++;
//					continue;
//				}
//				else
//				{
//					it3 = it;
//					it++;
//					diffs.splice(it2,diffs,it3);
//				}
//			}
//		}
//		if(it==diffs.end())
//			break;
//	}
//}


int main()
{
	string filename1;
	string filename2;
	clock_t begin,end;
	//begin = clock();
	/*cout<<"输入第一个文件的路径：";
	cin>>filename1;*/
	FileRead("in.txt",file1);

	/*cout<<"输入第二个文件的路径：";
	cin>>filename2;*/
	FileRead("in2.txt",file2);
	//end = clock();
	//cout<<end-begin<<endl;
	
	begin = clock();
	cmp_files(file1,file1.size(),file2,file2.size());
	end = clock();
	cout<<end-begin<<endl;
	cout<<"first end"<<endl;

	sortDiffs();//remanage the diff result
	cout<<"second end"<<endl;

	freopen("res.patch","w",stdout);
	list<Diff*>::iterator it = diffs.begin();
	cout<<"--------------------"<<endl;
	for(;it != diffs.end(); it++)
	{
		for(int i=0; i<(*it)->text.size();i++)
		{
			if((*it)->operation == INSERT)
				cout<<"+|";
			else if((*it)->operation == DELETE)
				cout<<"-|";
			else 
				cout<<" |";
			cout<<intToString[int((*it)->text[i])]<<endl;
		}
	}

	cout<<"--------------------"<<endl;
	return 0;
}


