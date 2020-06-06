#include <iostream>        
#include<fstream>          
#include<algorithm>
#include <vector>         
#include<unordered_map>
#include<set>
#include<string>
#include<time.h> 
#include<queue>      
using namespace std;



const int min_limit_circle_length = 3;
const int max_limit_circle_length = 8;

vector<string> ans(6);     
int cycles_size = 0;

vector<string> int_to_str;			

vector<vector<pair<int,double>>>out;					

vector<int> indegree;
vector<int> outdegree;											
vector<int> visited;
vector<int> can_dfs;                   

void read_data_from_file(string input_filename);      
void save_data(string output_filename);            
void find_all_circles();
void dfs(int &right, double &rightEdge, int& start, std::vector<int> & path, double & firstEdge);

int main()
{
	
	read_data_from_file("/data/test_data.txt");
	find_all_circles();
	save_data("/projects/student/result.txt");
	return 0;
	
}
void read_data_from_file(string input_filename)      
{
	set<double>vertexs;
	vector<double> edges;

	int file_length = 0;
	FILE* fp = fopen(input_filename.c_str(), "rb");
	fseek(fp, 0L, SEEK_END);
	file_length = ftell(fp);
	rewind(fp);
	auto ptr = new char[file_length];
	fread(ptr, 1, file_length, fp);
	fclose(fp);
	short intPlaceHolder[10];
	int lastP = -1;
	int loc = 0;
	unsigned int tenMag[10] = { 1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000 };
	double t[2] = { 0.1,0.01 };
	if (ptr)
	{
		size_t fLengthinChar = file_length / sizeof(char);
		for (int i = 0; i < fLengthinChar; ++i) {
			if (ptr[i] < 48 || ptr[i] > 57) {
				if (lastP != -1) {
					unsigned int newI = 0;
					for (int j = 0; j < lastP + 1; ++j) {
						newI += intPlaceHolder[j] * tenMag[lastP - j];
					}
					
					if (++loc == 3)
					{
						loc = 0;
					}
					else
					{
						vertexs.insert(newI);
					}

					if (ptr[i] == '.')
					{
						vector<unsigned int>rec;
						++i;
						while (ptr[i] >= 48 && ptr[i] <= 57)
						{
							rec.push_back(ptr[i] - 48);
							++i;
						}
						for (int jj = 0; jj < rec.size(); ++jj)
						{
							newI += rec[jj] * t[jj];
						}
					}

					edges.push_back(newI);
					lastP = -1;
				}
				continue;
			}
			intPlaceHolder[++lastP] = ptr[i] - 48;
		}
	}
	delete[]ptr;

	unordered_map<double, int> number_to_index; 
	int vertexs_size  = vertexs.size();        
	int_to_str.reserve(vertexs_size);
	out.resize(vertexs_size);
	indegree.resize(vertexs_size);
	outdegree.resize(vertexs_size);
	visited.resize(vertexs_size);
	can_dfs.resize(vertexs_size);

	int index = 0;
	for (auto vertex : vertexs)
	{
		number_to_index[vertex] = index++;
		int_to_str.push_back(to_string(static_cast<int>(vertex)));
	}

	vector<vector<int>> outc, inc;	
	int edge_size = edges.size();
	for (int i = 0; i < edge_size; i+=3)
	{	
		int first = number_to_index[edges[i]];
		int second = number_to_index[edges[i+1]];
		double third = edges[i+2];
		outc[first].push_back(second);
		inc[second].push_back(first);
		indegree[second]++;
		outdegree[first]++;
	}


	queue<unsigned int>q,p;
	for (auto payer= 0; payer<vertexs_size;++payer)
	{
		if (outdegree[payer] == 0)
		{
			q.push(payer);
		}
	}
	while (!q.empty())
	{
		unsigned int payer = q.front();
		q.pop();
		for (auto& payee : outc[payer])
		{
			if ((--outdegree[payee]) == 0) q.push(payee);
		}
	}

	for (auto payer= 0; payer<vertexs_size;++payer)
	{
		if (indegree[payer] == 0)
		{
			p.push(payer);
		}
	}
	while (!p.empty())
	{
		unsigned int payer = p.front();
		p.pop();
		for (auto& payee : inc[payer])
		{
			if ((--indegree[payee]) == 0) p.push(payee);
		}
	}
	for (int i = 0; i < edge_size; i+=3)
	{	
		if (indegree[i] == 0 || outdegree[i] == 0)
		{
			/* code */

			int first = number_to_index[edges[i]];
			int second = number_to_index[edges[i+1]];
			double third = edges[i+2];
			out[first].push_back(make_pair(second, third));

		}
	}		
}

void save_data(string output_filename)   
{
	ofstream fout(output_filename);
	fout << cycles_size << endl;
	for (int i = 0; i < max_limit_circle_length - min_limit_circle_length; ++i)
	{
		fout<<ans[i];
	}
    fout.close();
}


void dfs(int & rightSecond, double & rightEdge, int& start, std::vector<int> & path, double & firstEdge)
{
	

	int size = path.size();
	if (size>max_limit_circle_length) return;


	for(auto it:out[rightSecond])
	{

		int next = get<0>(it);
		if (next<start) continue;
		// if (indegree[next] == 0 || outdegree[next] == 0) continue;
		

	    double nextEdge = get<1>(it);
	    double eLimit = nextEdge/rightEdge;

	    if (eLimit>=0.2 && eLimit<=3 && visited[next] == 0)
	    {
			if (next>start) 
				{
					visited[next] = 1;
					path.push_back(next);
				    dfs(next, nextEdge, start, path, firstEdge);
					visited[next] = 0;
					path.pop_back();
				}
			else
			{
				double confirmLimit = firstEdge/nextEdge;
				if(confirmLimit>=0.2 && confirmLimit<=3)
				{
					/* code */
					string line = "";
					for (int k = 0; k < size; ++k)
					{
						line += int_to_str[path[k]];
						if (k < size - 1) line += ',';
						else line += '\n';
					}
					++cycles_size;
					ans[size - min_limit_circle_length] += line;
				}
			}
	    }

	}

}





void find_all_circles()
{

	for (int i = 0; i < visited.size(); ++i)
	{	

		// if (indegree[i] == 0 || outdegree[i] == 0) continue;

			for(auto it1:out[i])
			{
				int rightFirst = get<0>(it1);
				// if (indegree[rightFirst] == 0 || outdegree[rightFirst] == 0) continue;

				for(auto it2:out[rightFirst])
				{
					int rightSecond = get<0>(it2); 
				    // if (indegree[rightSecond] == 0 || outdegree[rightSecond] == 0) continue;

				    
				    double rightFirstEdge = get<1>(it1);
				    double rightSecondEdge = get<1>(it2);

				    double edgeLimit = rightSecondEdge/rightFirstEdge;

					if(i<rightFirst && i< rightSecond && rightFirst != rightSecond && edgeLimit >=0.2 && edgeLimit<=3)
					{	
						std::vector<int> path = {i, rightFirst, rightSecond};
						visited[rightFirst] = 1; visited[rightSecond] = 1;
						dfs(rightSecond, rightSecondEdge, i, path, rightFirstEdge);
						visited[rightFirst] = 0; visited[rightSecond] = 0;
					}
				}
			}
	}
}

