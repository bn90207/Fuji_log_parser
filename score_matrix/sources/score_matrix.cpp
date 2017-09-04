#include<iostream>
#include<fstream>
#include<dirent.h>
#include<string>
#include<vector>
#include<sys/stat.h>//stat(), S_ISDIR()
#include<algorithm>//remove_if, find
#include<boost/filesystem.hpp>//remove_all
#include <unistd.h>//rmdir
#include"windows.h"//CreateDirectory
using namespace std;

string dest_path = "D:\\Project\\template_matching\\Fuji Log\\classifier\\result";

bool is_dir(string);
int get_dir_list(string, vector<string>&);
int get_tif_number(string);
void read_score(string, string, int, int, vector< vector<double> >&);
void output_score(string, int, vector< vector<double> >&);
void remove_old_group(string);
void group(string, int, vector< vector<double> >&);
//void remove_all(string);

bool is_dir(string path)
{
	struct stat sb;
	if(stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
	
	return false;	
}

int get_dir_list(string path, vector<string>& dir_list)
{
	//Check wether the path represent a directory.
	if(is_dir(path))
	{
		DIR* dir;
		struct dirent* ent;
		if((dir = opendir(path.c_str())) != NULL)
		{
			while((ent = readdir(dir)) != NULL)
			{
				string file_name = ent->d_name;
				if(file_name == "." || file_name == ".."){}
				else if(is_dir(path+"\\"+file_name))
					dir_list.push_back(path+"\\"+file_name);
			}			
		}		
	}
	else
		return -1;
	return 0;
}

int get_tif_number(string path)
{
	DIR *dir;
	struct dirent *ent;
	int tif_number = 0;
	if((dir = opendir(path.c_str())) != NULL)
	{
		while((ent = readdir(dir)) != NULL)
		{
			string file_name = ent->d_name;
			if(file_name.find(".tif") != string::npos)
				++tif_number;
		}
	}
	return tif_number;
}

void read_score(string path, string file_name, int template_number, int total_number, vector< vector<double> >& score_mat)
{
	ifstream score_log;
	score_log.open((path+"\\"+file_name).c_str());
	
	if(score_log)
	{
		cout << "read " << path << "\\" << file_name << " success." << endl;
		string row;
		//Throw out the header
		getline(score_log, row);
		
		int image_number;
		double score;
		//Get score for each template;
		while(getline(score_log, row))//Since we seperate numbers by comma, we can't use "while(score_log >> i && score_log >> j)" to retrieve image-number and score in the same row.
		{
			size_t pos = row.find(',');
			image_number = stoi(row.substr(0, pos));
			score = stod(row.substr(pos+1));
			
			score_mat[image_number][template_number] = score;
		}
	}
	else
		cout << "read " << path << "\\" << file_name << "fail." << endl;
	score_log.clear();
	return;
}

void output_score(string path, int total_number, vector< vector<double> >& score_mat)
{
	ofstream out_mat;
	out_mat.open((path+"\\score_matrix.csv").c_str());
	
	for(int row=0; row<=total_number; ++row)
	{
		for(int col=0; col<=total_number; ++col)
			col==total_number? out_mat<<score_mat[row][col]: out_mat<<score_mat[row][col]<<",";
		out_mat << endl;
	}
	out_mat.close();
}
/*
void remove_all(string path)
{
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(path.c_str())) != NULL)
	{
		while((ent = readdir(dir)) != NULL)
		{
			string file_name = ent->d_name;
			if(file_name == "." || file_name == ".."){}
			else if(is_dir(path+"\\"+file_name))
				remove_all(path+"\\"+file_name);
			else
				remove((path+"\\"+file_name).c_str());
		}
	}
	rmdir(path.c_str());
}*/

void remove_old_group(string path)
{
	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(path.c_str())) != NULL)
	{
		while((ent = readdir(dir)) != NULL)
		{
			string file_name = ent->d_name;
			if(file_name.find("group") != string::npos)
			{
				boost::filesystem::remove_all(boost::filesystem::path(path+"\\"+file_name));
				/*
				if(is_dir(path+"\\"+file_name))
					remove_all((path+"\\"+file_name).c_str());
				else
					remove((path+"\\"+file_name).c_str());
				*/
			}
		}
	}
	return;
}

void group(string path, int total_number, vector< vector<double> >& score_mat)
{
	remove_old_group(path);
	int group_number = 0;
	double threshold = 0.9;
	
	vector<int> ungroup_member;
	ungroup_member.reserve(total_number);
	for(int i=1; i<score_mat.size(); ++i)
		ungroup_member.push_back((int)score_mat[0][i]);	
	while(ungroup_member.size() > 0)
	{
		++group_number;
		vector<int> group_member;
		group_member.reserve(ungroup_member.size());
		//Initailize group_member;
		for(vector<int>::iterator iter=ungroup_member.begin(); iter!=ungroup_member.end(); ++iter)
			group_member.push_back(*iter);
		
		//Remove the objects with score comparing to subject less than threshold.
		for(vector<int>::iterator subject=group_member.begin(); subject!=group_member.end(); ++subject)
		{
			for(vector<int>::iterator object=subject+1; object!=group_member.end(); ++object)
			{
				if(score_mat[*subject][*object] < threshold || score_mat[*object][*subject] < threshold)
					object = group_member.erase(object) - 1;//erase() will return an iterator pointing to the element followed the removed one.	
			}
			
		}

		//Create an matrix to store similar members.
		vector< vector<double> > group_mat(group_member.size()+1, vector<double>(group_member.size()+1));
		for(int col=0; col<=group_member.size(); ++col)
		{
			for(int row=0; row<=group_member.size(); ++row)
			{
				if(col == 0 && row == 0)
					continue;
				else if(col == 0 || row == 0)//Fill the first row and column.
					group_mat[row][col] = group_member[row+col-1];
				else
					group_mat[row][col] = score_mat[group_member[row-1]][group_member[col-1]];
			}
		}

		//Output group matrix.
		string dir = path+"\\group_"+to_string(group_number);
		CreateDirectory(dir.c_str(), NULL);
		ofstream out_mat;
		out_mat.open((dir+"\\group_"+to_string(group_number)+".csv").c_str());
		for(int row=0; row<group_mat.size(); ++row)
		{
			for(int col=0; col<group_mat.size(); ++col)
				col==group_mat.size()-1? out_mat<<group_mat[row][col]: out_mat<<group_mat[row][col]<<",";
			out_mat << endl;
		}
		out_mat.close();
		
		//Create a directory to store similar mark
		for(vector<int>::iterator iter=group_member.begin(); iter!=group_member.end(); ++iter)
		{
			ifstream source;
			source.open((path+"\\"+to_string(*iter)+".tif").c_str(), ios_base::binary);
			ofstream dest;
			dest.open((dir+"\\"+to_string(*iter)+".tif").c_str(), ios_base::binary);
			dest << source.rdbuf();
			source.close();
			dest.close();
		}
		
		//Remove grouped member from ungroup member.
		ungroup_member.erase(remove_if(ungroup_member.begin(), ungroup_member.end(), [&](int member){
			return find(group_member.begin(), group_member.end(), member)!=group_member.end();
		}), ungroup_member.end());

		group_member.clear();
	}
}


int main(){
	
	vector<string> dir_list;
	dir_list.reserve(8);//Do not use resize. it may cause problem when iterating vector if the elements pushed in were less than the resize() reserved.
	
	get_dir_list(dest_path, dir_list);
	for(vector<string>::iterator iter=dir_list.begin(); iter!=dir_list.end(); ++iter)
	{
		cout << *iter << endl;
		//Create a 2D vector to store score for each template.
		int mark_number = get_tif_number(*iter);
		vector< vector<double> > score_mat(mark_number+1, vector<double>(mark_number+1));//Use vector is much better than array because in the spirit of array its size should be fixed in compiling time.
		for(int i=1; i<=mark_number; ++i)//Initialize first row and column. Remeber the column index represents the template and the row index is the image.
		{
			score_mat[i][0] = i;
			score_mat[0][i] = i;
		}
		
		//Read each score-logs
		for(int i=1; i<=mark_number; ++i)
		{
			string score_log = to_string(i) + "(score).csv";
			read_score(*iter, score_log, i, mark_number, score_mat);
		}
		output_score(*iter,  mark_number, score_mat);
		group(*iter, mark_number, score_mat);
	}//After leaving the block, vector would be automatically released.	

} 
