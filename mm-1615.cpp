#include <iostream>        //查看和调试
#include<fstream>          //读取txt文件
#include<algorithm>
#include <vector>         
#include<string>
#include<queue>
#include<map>
#include<unordered_map>
#include<time.h>       //计算程序运行时间
using namespace std;

const int min_limit_circle_length = 3;
const int max_limit_circle_length = 8;
const double min_limit_money_ratio = 0.2;
const double max_limit_money_ratio = 3;
inline bool check_ratio(double r)
{
	return r >= min_limit_money_ratio && r <= max_limit_money_ratio;
}
//有向图
vector<string>ans(6);     //保存环,按照长度不同分开  
unsigned int  cycles_size = 0;               //环的数目
unsigned int payer_size = 1;                //从1开始编号
vector<vector<pair<unsigned int, double>>>pay;					//入边
vector<vector<pair<unsigned int, double>>>accept;					// 出边
vector<unsigned int>path;						//栈
vector<int>visited;							 //0：不在栈中，1：在栈中，-1：该点出度或入度为0或不存在该点
vector<int>can_go_back;                     //保存剪枝的结果，-1代表不在环上，
												//正数x代表该点可能属于顶点为x的环的后3个点
vector<string> order_to_number;			//index_to_number[index]代表第index个数字对应的用户ID数字,index：[1,无穷)

void read_data_from_file(string input_filename);      //从txt文件读取数据
void show_data();									//显示数据
void save_data(string output_filename);             //写数据到txt文件
void find_all_circles();
void dfs(unsigned int this_vertex, double this_money, double money);

int main()
{
	read_data_from_file("/data/test_data.txt");
	find_all_circles();
	save_data("/projects/student/result.txt");
	return 0;
}
void read_data_from_file(string input_filename)      //从txt文件读取数据
{
	vector<double> edges;
	edges.reserve(6000000);

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
	double tenMag[10] = { 1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000 };
	double t[2] = { 0.1,0.01 };
	if (ptr)
	{
		size_t fLengthinChar = file_length / sizeof(char);
		for (int i = 0; i < fLengthinChar; ++i) {
			if (ptr[i] < 48 || ptr[i] > 57) {
				if (lastP != -1) {
					double newI = 0;
					for (int j = 0; j < lastP + 1; ++j) {
						newI += intPlaceHolder[j] * tenMag[lastP - j];
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
	map<unsigned int, vector<pair<unsigned int, double>>>graph;
	unordered_map<unsigned int, unsigned int>in_degree;
	int edge_size = edges.size();
	for (int i = 0; i < edge_size; i += 3)
	{
		unsigned int payer = static_cast<unsigned int>(edges[i]);
		unsigned int payee = static_cast<unsigned int>(edges[i + 1]);
		graph[payer].emplace_back(make_pair(payee, edges[i + 2]));
		in_degree[payee] += 1;
	}
	queue<unsigned int>q;
	for (auto& payer : graph)
	{
		if (in_degree[payer.first] == 0)
		{
			q.push(payer.first);
		}
	}
	while (!q.empty())
	{
		unsigned int payer = q.front();
		q.pop();
		for (auto& payee : graph[payer])
		{
			if ((--in_degree[payee.first]) == 0) q.push(payee.first);
		}
	}
	unordered_map<unsigned int, unsigned int>number_to_order;
	for (auto& payer : graph)
	{
		if (in_degree[payer.first]) number_to_order[payer.first] = payer_size++;
	}
	pay.resize(payer_size);
	accept.resize(payer_size);
	visited.resize(payer_size);
	can_go_back.resize(payer_size);
	order_to_number.resize(payer_size);

	for (auto& payer : graph)
	{
		if (in_degree[payer.first])
		{
			unsigned int p = number_to_order[payer.first];
			for (auto& payee : graph[payer.first])
			{
				if (in_degree[payee.first])
				{
					unsigned int a = number_to_order[payee.first];
					pay[p].emplace_back(make_pair(a, payee.second));
					accept[a].emplace_back(make_pair(p, payee.second));
				}
			}
			order_to_number[p] = to_string(payer.first);
		}
	}

	for (int i = 1; i < payer_size; i++)
	{
		sort(pay[i].begin(), pay[i].end());
	}
}

void show_data()
{
	cout << cycles_size << endl;
}
void save_data(string output_filename)    //写数据到txt文件
{
	ofstream fout(output_filename);
	fout << cycles_size << endl;
	for (int i = 0; i <= max_limit_circle_length - min_limit_circle_length; ++i)
	{
		fout << ans[i];
	}
	fout.close();
}
void find_prev_four_points(unsigned int this_vertex)
{
	can_go_back[this_vertex] = this_vertex;
	visited[this_vertex] = 1;
	for (auto& prev_first_vertex : accept[this_vertex])
	{
		if (prev_first_vertex.first > this_vertex && visited[prev_first_vertex.first] == 0)
		{
			can_go_back[prev_first_vertex.first] = this_vertex;
			visited[prev_first_vertex.first] = 1;

			for (auto& prev_second_vertex : accept[prev_first_vertex.first])
			{
				if (prev_second_vertex.first > this_vertex && visited[prev_second_vertex.first] == 0)
				{
					can_go_back[prev_second_vertex.first] = this_vertex;
					visited[prev_second_vertex.first] = 1;

					for (auto& prev_third_vertex : accept[prev_second_vertex.first])
					{
						if (prev_third_vertex.first > this_vertex && visited[prev_third_vertex.first] == 0)
						{
							can_go_back[prev_third_vertex.first] = this_vertex;
							visited[prev_third_vertex.first] = 1;
							for (auto& prev_fourth_vertex : accept[prev_third_vertex.first])
							{
								if (prev_fourth_vertex.first > this_vertex && visited[prev_fourth_vertex.first] == 0)
								{
									can_go_back[prev_fourth_vertex.first] = this_vertex;
								}
							}
							visited[prev_third_vertex.first] = 0;
						}
					}

					visited[prev_second_vertex.first] = 0;
				}
			}

			visited[prev_first_vertex.first] = 0;
		}
	}
	visited[this_vertex] = 0;
}
void get_cycles(unsigned int this_vertex, double this_money, double money)
{
	if (this_vertex == path[0])
	{
		if (path.size() >= min_limit_circle_length)
		{
			double ratio = money / this_money;
			if (check_ratio(ratio))
			{
				string line = "";
				for (int k = 0; k < path.size(); ++k)
				{
					line += order_to_number[path[k]];
					if (k < path.size() - 1) line += ',';
					else line += '\n';
				}
				++cycles_size;
				ans[path.size() - min_limit_circle_length] += line;
				return;
			}
		}

	}
	visited[this_vertex] = 1;
	path.push_back(this_vertex);
	int n = path.size();
	for (auto& next_vertex : pay[this_vertex])
	{

		if (visited[next_vertex.first] == 0)
		{
			if (can_go_back[next_vertex.first] == path[0] && n <= max_limit_circle_length)
			{
				double ratio = next_vertex.second / this_money;
				if (check_ratio(ratio))
				{
					get_cycles(next_vertex.first, next_vertex.second, money);
				}
			}
			else if (next_vertex.first > path[0] && n <= min_limit_circle_length)
			{
				double ratio = next_vertex.second / this_money;
				if (check_ratio(ratio))
				{
					dfs(next_vertex.first, next_vertex.second, money);
				}

			}
		}

	}
	path.pop_back();
	visited[this_vertex] = 0;

}
void dfs(unsigned int this_vertex, double this_money, double money)
{
	if (path.size() > 3) return;
	path.push_back(this_vertex);
	visited[this_vertex] = 1;

	for (auto& next_vertex : pay[this_vertex])
	{
		if (visited[next_vertex.first] == 0)
		{
			if (can_go_back[next_vertex.first] == path[0])
			{
				double ratio = next_vertex.second / this_money;
				if (check_ratio(ratio))
				{
					get_cycles(next_vertex.first, next_vertex.second, money);
				}
			}
			else if (next_vertex.first > path[0])
			{
				double ratio = next_vertex.second / this_money;
				if (check_ratio(ratio))
				{
					dfs(next_vertex.first, next_vertex.second, money);
				}
			}
		}
	}
	visited[this_vertex] = 0;
	path.pop_back();
}

void find_all_circles()
{
	path.reserve(10);
	for (int i = 1; i < payer_size; ++i)
	{
		find_prev_four_points(i);
		path.push_back(i);
		for (auto& next_vertex : pay[i])
		{
			if (next_vertex.first > path[0])
			{
				dfs(next_vertex.first, next_vertex.second, next_vertex.second);
			}
		}
		path.pop_back();
	}
}