#include<iostream>
#include<fstream>
#include<string>
#include<dirent.h>
#include<sys/stat.h>//stat(), S_ISDIR()
#include<vector>
#include<algorithm>
#include <cctype>
#include "windows.h"
#include<stdio.h>
#include "classifier.h"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace std;
using namespace cv;

class log_content;
bool read_row(string, string, string [], int, int [] = NULL, const int = 0);
void output(string, string, vector<log_content>&);
long file_size(string, string);
bool is_dir(string);
bool file_exist(string, string);
int get_dir_list(string, vector<string>&, int);
void read_log(string, vector<log_content>&, const int&);
bool compare_mark(string, string);
string get_template_name(const string&);
string get_template_name(log_content&);
template <class T>
bool match_mark(string, string, string, vector<T>&, typename vector<T>::iterator&);
int get_tif_number(string);
void copy_mark(string, string, string, string);


const int get_DN_UP = 0;
const int get_lots = 1; 
const int un_classified = 0;
const int classified = 1;
const int get_position = 0;
const int get_content = 1; 
const string unclassified_log_name = "template_log.csv";
const string classified_log_name = "classified_log.csv";

bool read_row(string row, string delimiter, string parameters[], int parameter_number, int parameter_position[], const int flag)
{
	if(row.length() == 0 || delimiter.length() == 0)
		return false;
	
	if(flag == get_position && parameter_position != NULL)
	{
		for(int i=0; i<parameter_number; ++i)
			parameter_position[i] = -1;
	}
	
	bool found = false;
	size_t pos = string::npos;
	int i = 0;
	do
	{
		pos = row.find(delimiter);
		string name = row.substr(0, pos);
		if(pos+1 <= row.length())
			row = row.substr(pos+1);//if pos == string::npos, pos+1 = 0
		else//if pos+1 > row.length(), set the row to empty, which will prevent the row was ended by delimiter
			row = "";
		for(int j=0; j<parameter_number; ++j)
		{
			if(flag == get_position && parameters[j] == name)
			{
				found = true;
				if(parameter_position != NULL)
					parameter_position[j] = i;
			}
			else if(flag == get_content)
			{
				if(parameter_position == NULL)
				{
					parameters[0] = name;
					return true;
				}
				else if(parameter_position[j] == i)
				{
					found = true;
					parameters[j] = name;
				}
			}
		}
		++i;
	}while(pos != string::npos);
	return found;
}



class log_content{
	public:
	string time;
	string job_name;
	string lot_number;
	string sequence_number;
	string layer;
	string templae_width;
	string template_height;
	string center_x;
	string center_y;
	string apply_prefilter_to_circle;
	string level_correct_on;
	string level_correct_min;
	string level_correct_max;
	string gamma;
	string threshold_high_res;
	string threshold_low_res;
	string mark_color;
	string circle_fitting_on;
	string m_est_sigma;
	string kind_circle_fit;
	string weak_edge;
	string strong_edge;
	string min_contrast;
	string pre_circle_fitting_on;
	string edge_detection;
	string diameter_circle_fit_circle;
	string margin_circle_fit_circle;
	string mark_num;
	string mark_num_threshold;
	string kind_assemble;
	string strobo_type;
	string strobo_control;
	string diameter_assemble_circle;
	string margin_assemble_circle;
	string diameter_circle_fit_ring;
	string margin_circle_fit_ring;
	string template_mask_on;
	string template_name;//string dest_filename = exp_time + "_" + product_name + "_" + lot_number + "_" + layer + "_" + pnl_number + ".tif"
 
 
 	log_content(){
 	 
 	time.clear();
	job_name.clear();
	lot_number.clear();
	sequence_number.clear();
	layer.clear();
	templae_width.clear();
	template_height.clear();
	center_x.clear();
	center_y.clear();
	apply_prefilter_to_circle.clear();
	level_correct_on.clear();
	level_correct_min.clear();
	level_correct_max.clear();
	gamma.clear();
	threshold_high_res.clear();
	threshold_low_res.clear();
	mark_color.clear();
	circle_fitting_on.clear();
	m_est_sigma.clear();
	kind_circle_fit.clear();
	weak_edge.clear();
	strong_edge.clear();
	min_contrast.clear();
	pre_circle_fitting_on.clear();
	edge_detection.clear();
	diameter_circle_fit_circle.clear();
	margin_circle_fit_circle.clear();
	mark_num.clear();
	mark_num_threshold.clear();
	kind_assemble.clear();
	strobo_type.clear();
	strobo_control.clear();
	diameter_assemble_circle.clear();
	margin_assemble_circle.clear();
	diameter_circle_fit_ring.clear();
	margin_circle_fit_ring.clear();
	template_mask_on.clear();
	template_name.clear();
	}
	 log_content(string head_row, string row, const int& flag):log_content(){
	 	int array_size = 38;
	 	string parameter_names[array_size] = {"time", "job_name", "lot_number", "sequence_number", "layer", "Templae size(W)", "Template size(H)", "Center of gravity(X)", "Center of gravity(y)", "Apply prefilter to circlefit", "Level correct on", "Level correct min", "Level correct max", "Gamma", "Threshold High Res", "Threshold Low Res", "Mark color", "Circle fitting on", "M-est sigma", "Kind(Circle fit)", "Weak edge", "Strong edge", "Min-constrat", "Pre circle fitting on", "Edge detection", "Diameter(Circle fit:Circle)", "Margin(Circle fit:Circle)", "Mark num", "Mark num(threshold)", "Kind(Assemble)", "Strobo type", "Strobo control", "Diameter(Assemble:Circle)", "Margin(Assemble:Circle)", "Diameter(Circle fit:Ring)", "Margin(Circle fit:Ring)", "Template mask on", "template_backup"};
	 	int parameter_position[array_size];
	 	string parameter_content[array_size];
	 	string *parameters[array_size] = {&time, &job_name, &lot_number, &sequence_number, &layer, &templae_width, &template_height, &center_x, &center_y, &apply_prefilter_to_circle, &level_correct_on, &level_correct_min, &level_correct_max, &gamma, &threshold_high_res, &threshold_low_res, &mark_color, &circle_fitting_on, &m_est_sigma, &kind_circle_fit, &weak_edge, &strong_edge, &min_contrast, &pre_circle_fitting_on, &edge_detection, &diameter_circle_fit_circle, &margin_circle_fit_circle, &mark_num, &mark_num_threshold, &kind_assemble, &strobo_type, &strobo_control, &diameter_assemble_circle, &margin_assemble_circle, &diameter_circle_fit_ring, &margin_circle_fit_ring, &template_mask_on, &template_name}; 	
	 	read_row(head_row, ",", parameter_names, array_size, parameter_position, get_position);
		read_row(row, ",", parameter_content, array_size, parameter_position, get_content);
		for(int i=0; i<array_size; ++i)
			*parameters[i] = parameter_content[i];
		if(flag == un_classified)
		{
			template_name = time+"_"+job_name+"_"+lot_number+"_"+layer+"_"+sequence_number+".tif";
			//Remove chars other than digit or alphbet from lot number and change the lower cases to upper cases.
			if(!lot_number.empty())
			lot_number.erase(remove_if(lot_number.begin(), lot_number.end(), [&](char s){	return !isalnum(s);}), lot_number.end());
		 	transform(lot_number.begin(), lot_number.end(), lot_number.begin(), ::toupper);
		}
		if(flag == classified)
			sequence_number = sequence_number + "(0)";
	}
	/*
	bool operator < (const log_content& right_operand) const
	{
		return stoi(template_name) < stoi(right_operand.template_name);
	}
	*/
	void display()
	{	
		cout << "time:" << time << endl
		<< "job name:" << job_name << endl
		<< "lot_number" << lot_number << endl
		<< "sequence:" << sequence_number << endl
		<< "layer:" << layer << endl
		<< "strobo type:" << strobo_type << endl
		<< "mark color:" << mark_color << endl
		<< "kind assemble:" << kind_assemble << endl
		<< "template name:" << template_name << endl; 
	}
	int get_new_sequence_number()
	{
		int old_pnls = 0, new_pnls = 0;
		sscanf(sequence_number.c_str(), "%d(%d)", &old_pnls, &new_pnls);
		return	new_pnls;
	}
	int get_old_sequence_number()
	{
		int old_pnls = 0, new_pnls = 0;
		sscanf(sequence_number.c_str(), "%d(%d)", &old_pnls, &new_pnls);
		return	old_pnls;
	}
	void update_sequence_number(int i)
	{
		sequence_number = to_string(get_old_sequence_number()) + "(" + to_string(get_new_sequence_number() + i) +")";
	}
	void output(ofstream& out_file)
	{
		sequence_number = to_string(get_old_sequence_number() + get_new_sequence_number());	
		out_file << time << "," << job_name << "," << lot_number << "," << sequence_number << "," << layer << "," << templae_width << "," << template_height << "," << center_x << "," << center_y << "," << apply_prefilter_to_circle << "," << level_correct_on << "," << level_correct_min << "," << level_correct_max << "," << gamma << "," << threshold_high_res << "," << threshold_low_res << "," << mark_color << "," << circle_fitting_on << "," << m_est_sigma << "," << kind_circle_fit << "," << weak_edge << "," << strong_edge << "," << min_contrast << "," << pre_circle_fitting_on << "," << edge_detection << "," << diameter_circle_fit_circle << "," << margin_circle_fit_circle << "," << mark_num << "," << mark_num_threshold << "," << kind_assemble << "," << strobo_type << "," << strobo_control << "," << diameter_assemble_circle << "," << margin_assemble_circle << "," << diameter_circle_fit_ring << "," << margin_circle_fit_ring << "," << template_mask_on << "," << template_name << endl;
	}					
};

void output(string dst_path, string file_name, vector<log_content>& output_list)
{
	ofstream out_file;
	out_file.open((dst_path+"\\"+file_name).c_str());
	out_file << "time,job_name,lot_number,sequence_number,layer,Templae size(W),Template size(H),Center of gravity(X),Center of gravity(y),Apply prefilter to circlefit,Level correct on,Level correct min,Level correct max,Gamma,Threshold High Res,Threshold Low Res,Mark color,Circle fitting on,M-est sigma,Kind(Circle fit),Weak edge,Strong edge,Min-constrat,Pre circle fitting on,Edge detection,Diameter(Circle fit:Circle),Margin(Circle fit:Circle),Mark num,Mark num(threshold),Kind(Assemble),Strobo type,Strobo control,Diameter(Assemble:Circle),Margin(Assemble:Circle),Diameter(Circle fit:Ring),Margin(Circle fit:Ring),Template mask on,template_backup" << endl;
	for(vector<log_content>::iterator iter=output_list.begin(); iter!=output_list.end(); ++iter)
		iter->output(out_file);
}

long file_size(string path, string file_name)
{
	struct stat sb;
	int i = stat((path+"\\"+file_name).c_str(), &sb);
	return i == 0? sb.st_size: 0;
}

bool is_dir(string path)
{
	struct stat sb;
	if(stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
		return true;
	
	return false;	
}

bool file_exist(string path, string file_name)
{
	struct stat sb;
	if(stat((path+"\\"+file_name).c_str(), &sb) == 0 && S_ISREG(sb.st_mode))
		return true;
		
	return false;
}

int get_dir_list(string path, vector<string>& dir_list, int flag)
{
	//Check wether the path represent a directory.
	if(is_dir(path))
	{
		//If path is a directory, read contents and list into dir_list if it was named "UP" or "DN".
		DIR* dir;
		struct dirent* ent;
		if((dir = opendir(path.c_str())) != NULL)
		{
			while((ent = readdir(dir)) != NULL)
			{
				string file_name = ent->d_name;
				if(file_name == "." || file_name == ".."){}
				else if(file_name == "DN" || file_name == "UP")
				{
					if(flag == get_DN_UP && is_dir(path+"\\"+file_name))
						dir_list.push_back(path+"\\"+file_name);
				}
				else
				{
					if(flag == get_lots && is_dir(path+"\\"+file_name))
						dir_list.push_back(path+"\\"+file_name);
					else
						get_dir_list(path+"\\"+file_name, dir_list, flag);
				}
			}			
		}		
	}
	else
		return -1;
	return 0;
}

void read_log(string path, vector<log_content>& log, const int& flag)
{
	cout << "read log:" << path << endl;
	ifstream in_file;
	in_file.open(path.c_str());
	
	string head_row;
	if(getline(in_file, head_row))
	{
		string row;
		while(getline(in_file, row))
			log.push_back(log_content(head_row, row, flag));
	}
}


bool compare_mark(string src_file, string dst_file)
{
	Mat src = imread(src_file.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	Mat dst = imread(dst_file.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	if(src.size() == dst.size())
	{
		Mat diff;
		bitwise_xor(src, dst, diff);
		if(countNonZero(diff) == 0)
			return true;
	}
	return false;
}

string get_template_name(const string& str)
{
	return str;
}

string get_template_name(log_content& log)
{
	//The template name in classified log will remove the .tif suffix
	return (log.template_name+".tif");
}

template <class T>
bool match_mark(string src_path, string src_file, string dst_path, vector<T>& dst_list, typename vector<T>::iterator& iter)
{
	if(dst_list.size() != 0)
	{
		//Compare to the last matched template first.
		if(compare_mark(src_path+"\\"+src_file, dst_path+"\\"+get_template_name(*iter)))
			return true;
		//If not match, compare to others and update last matched template when matching.
		for(typename vector<T>::iterator i=dst_list.begin(); i<dst_list.end(); ++i)
		{
			if(i != iter && compare_mark(src_path+"\\"+src_file, dst_path+"\\"+get_template_name(*i)))
			{
				iter = i;
				return true;
			}
		}
	}	
	return false;
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

void copy_mark(string src_path, string src_name, string dst_path, string dst_name)
{
	ifstream in_file;
	in_file.open((src_path+"\\"+src_name).c_str(), ios_base::binary);
	ofstream out_file;
	out_file.open((dst_path+"\\"+dst_name).c_str(), ios_base::binary);
	out_file << in_file.rdbuf();
	in_file.close();
	out_file.close();
}

int main(){
	
	string root_dir = "D:\\Project\\template_matching\\Fuji Log\\classifier\\test";	
	string result_dir = "D:\\Project\\template_matching\\Fuji Log\\classifier\\result";
	vector<string> dir_list;
	
	//get all directory named "DN" or "UP"
	get_dir_list(root_dir, dir_list, get_DN_UP);
	
	//get all directory under "DN" or "UP"
	for(vector<string>::iterator machine_dir_iter=dir_list.begin(); machine_dir_iter!=dir_list.end(); ++machine_dir_iter)
	{
		vector<string> lot_list;
		get_dir_list(*machine_dir_iter, lot_list, get_lots);
	
		for(vector<string>::iterator lots_dir_iter=lot_list.begin(); lots_dir_iter!=lot_list.end(); ++lots_dir_iter)
		{
			//Check wether the unclassified log exist.
			vector<log_content> unclassified_log;
			if(file_exist(*lots_dir_iter, unclassified_log_name))
				read_log(*lots_dir_iter+"\\"+unclassified_log_name, unclassified_log, un_classified);
			//Check wether the classified log exist.	
			vector<log_content> classified_log;
			if(file_exist(*lots_dir_iter, classified_log_name))
				read_log(*lots_dir_iter+"\\"+classified_log_name, classified_log, classified);
			//Create a iterator points to the last matched template	
			vector<log_content>::iterator matched_mark = classified_log.begin();
			//iterative compare to the template listed in classified log
			for(vector<log_content>::iterator unclassified_iter=unclassified_log.begin(); unclassified_iter!=unclassified_log.end(); ++unclassified_iter)
			{	
				unclassified_iter->display();
				cout << endl;
				if(match_mark(*lots_dir_iter, unclassified_iter->template_name, *lots_dir_iter, classified_log, matched_mark))//If match, add the recorded panel number and lot number
				{
					matched_mark->update_sequence_number(1);
					if((unclassified_iter->lot_number).length() != 0 && !read_row(matched_mark->lot_number, " ", &(unclassified_iter->lot_number), 1))//argument3: pass a reference of a string variable as a 1 element string array.
					{
						if((matched_mark->lot_number).length() != 0)
							matched_mark->lot_number = matched_mark->lot_number + " ";
						matched_mark->lot_number = matched_mark->lot_number + unclassified_iter->lot_number;
					}
					remove((*lots_dir_iter+"\\"+unclassified_iter->template_name).c_str());
				}
				else//If not match, push a new template into the classfied log and give it a new template number.
				{
					classified_log.push_back(*unclassified_iter);
					classified_log.back().sequence_number = "0(1)";
					classified_log.back().template_name = to_string(classified_log.size());//The template name in classified log will remove the .tif suffix
					matched_mark = classified_log.end(); --matched_mark;//After push_back, there may be relocation happened. So the iterator should be updated to new address.
					rename((*lots_dir_iter+"\\"+unclassified_iter->template_name).c_str(), (*lots_dir_iter+"\\"+classified_log.back().template_name+".tif").c_str());			
				}					
			}
			remove((*lots_dir_iter+"\\"+unclassified_log_name).c_str());
			//sort(classified_log.begin(), classified_log.end());	
				
					
			for(vector<log_content>::iterator classified_iter=classified_log.begin(); classified_iter!=classified_log.end(); ++classified_iter)
			{
				CreateDirectory(result_dir.c_str(), NULL);
				string dst_dir = result_dir+"\\"+classified_iter->strobo_type+"_"+classified_iter->mark_color+"_"+classified_iter->kind_assemble;
				CreateDirectory((dst_dir).c_str(), NULL);
				int mark_number = get_tif_number(dst_dir);
				bool found_equal = false;
				//Compare classified mark with the mark in result directory.
				for(int i=1; i<=mark_number; ++i)
				{
					if(compare_mark(*lots_dir_iter+"\\"+classified_iter->template_name+".tif", dst_dir+"\\"+to_string(i)+".tif"))//If it's matched...
					{
						found_equal = true;
						vector<log_content> mark_log;
						read_log(dst_dir+"\\"+to_string(i)+"(log).csv", mark_log, classified);
						vector<log_content>::iterator mark_log_iter=mark_log.begin();
						for(; mark_log_iter!=mark_log.end(); ++mark_log_iter)
						{
							if(mark_log_iter->job_name==classified_iter->job_name && mark_log_iter->layer==classified_iter->layer)//If find the same job name and layer in mark log...
							{							
								mark_log_iter->update_sequence_number(classified_iter->get_new_sequence_number());
								//Complex lot number compare start...
								if((classified_iter->lot_number).length() != 0)
								{
									if((mark_log_iter->lot_number).length() == 0)
										mark_log_iter->lot_number = classified_iter->lot_number;
									else
									{
										string row = classified_iter->lot_number;
										while(row.length() != 0)
										{
											string lot_number;
											//get the first lot number in classified log.
											read_row(row, " ", &lot_number, 1, NULL, get_content);
											//If the lot number was not found in mark log
											if(lot_number.length() != 0 && !read_row(mark_log_iter->lot_number, " ", &lot_number, 1))
												mark_log_iter->lot_number = mark_log_iter->lot_number + " " + lot_number;
											//If row's length is larger than lot number's length, it means there are more. Then subtract the lot number from row.
											if(lot_number.length()+1 < row.length())//+1 represent the length of delimiter
												row = row.substr(lot_number.length()+1);
											else
												break;
										}
									}	
								}
								//Complex lot number compare end...
								break;
							}
						}
						if(mark_log_iter == mark_log.end())//If the same job name and layer in mark log was not found...
						{
							mark_log.push_back(*classified_iter);
							mark_log.back().template_name = to_string(i);
						}
						output(dst_dir, to_string(i)+"(log).csv", mark_log);
						break;
					}
				}
				
				//If the classified mark was not matched to any mark in result directory
				if(found_equal == false)
				{
					++mark_number;
					//Copy mark to the result directory and rename it.
					copy_mark(*lots_dir_iter, classified_iter->template_name+".tif", dst_dir, to_string(mark_number)+".tif");
					//Create its own mark log.
					vector<log_content> mark_log;
					mark_log.push_back(*classified_iter);
					mark_log.back().template_name = to_string(mark_number);
					output(dst_dir, to_string(mark_number)+"(log).csv", mark_log);
					//Calculate the score of template matching with the original marks
					ofstream new_mark;
					new_mark.open((dst_dir+"\\"+to_string(mark_number)+"(score).csv").c_str());
					new_mark << "template_name,score" << endl;
					for(int i=1; i<=mark_number; ++i)
					{
						//cout << flush;
						double score = 0;
						//The score of mark i recorded means use mark i as template to match the other marks.
						if(i != mark_number)
						{
							cout << "Comparing " << i << "(template) to " << mark_number << "(image)..." << endl;
							score = mark_match(dst_dir+"\\"+to_string(mark_number)+".tif", dst_dir+"\\"+to_string(i)+".tif", dst_dir+"\\"+to_string(i)+"(match_image)"+"\\"+to_string(mark_number)+".tif");
							cout << "score:" << score << endl;
							ofstream old_mark;
							old_mark.open((dst_dir+"\\"+to_string(i)+"(score).csv").c_str(), ios_base::out|ios_base::app);
							old_mark << mark_number << "," << score << endl;
							old_mark.close();
						}
						
						CreateDirectory((dst_dir+"\\"+to_string(mark_number)+"(match_image)").c_str(), NULL);
						cout << "Comparing " << mark_number << "(image) to " << i << "(image)..." << endl;
						score = mark_match(dst_dir+"\\"+to_string(i)+".tif", dst_dir+"\\"+to_string(mark_number)+".tif", dst_dir+"\\"+to_string(mark_number)+"(match_image)"+"\\"+to_string(i)+".tif");
						cout << "score:" << score << endl;
						new_mark << i << "," << score << endl;
					}					
					new_mark.close();
				}
			}
			output(*lots_dir_iter, classified_log_name, classified_log);//Can't output before mark match because the sequence number will be changed.			
		}		
	}
}
