#include<iostream>
#include<fstream>
#include<string>
#include "dirent.h"
#include "windows.h" //_sleep()
#include<stdlib.h> // atoi()
#include<ctime> //time(), localtime(), difftime()
#include"tinyxml2.h" //xml parser
#include<vector>
#include<cstring>//memcpy
#include<limits>//std::numeric_limits< double >::is_iec559
#include<sstream> 
using namespace std;
using namespace tinyxml2;

union data;
enum Type{Uint32_t,	Double};
enum Side{up, down};
class tag;
void Initialize_tags(vector<tag>&);
void Output(vector<tag>&,const string, const string, const string, const string, const string, const string,const string, const string);
template<class T, size_t N>
size_t array_size(T (&)[N]);

union data{
	uint32_t x;
	double y;
	data(){
		x = 0;
		y = 0.0;
	}
};

class tag{
	public:
	int tag_ID;
	string tag_name;
	Type type;
	data tag_content;
	tag():tag_ID(0), type(Uint32_t), tag_content(){
	}
	tag(int i, string str):tag_ID(i), tag_name(str), type(Uint32_t), tag_content(){
	}
};

void Initialize_tags(vector<tag>& tags){
	for(vector<tag>::iterator iter=tags.begin(); iter!=tags.end(); ++iter)
	{
		iter->type = Uint32_t;
		(iter->tag_content).x = 0;
		(iter->tag_content).y = 0.0;
	}
	
}

void Output(vector<tag>& tags,const string exp_time, const string product_name, const string lot_number, const string pnl_number, const string layer, const string job_ID,const string template_file_path,  const string out_file_path){
	ifstream check_file_size;
	check_file_size.open(out_file_path.c_str(), ios_base::binary|ios_base::ate); //ate: if file doesn't exist, create an empty file
	int file_size = check_file_size.tellg();
	check_file_size.close();
	ofstream out_file;
	out_file.open(out_file_path.c_str(), ios_base::out|ios_base::app);
	if(file_size <= 0)
	{
		out_file << "time,job_name,lot_number,sequence_number,layer,job ID,";
		for(vector<tag>::iterator iter=tags.begin(); iter!=tags.end(); ++iter)
		{
			out_file << iter->tag_name << ',';
			//(iter + 1) == tags.end()? out_file << endl : out_file << ','; 
		}
		out_file << "template_backup" << endl;
	}
	
	out_file << exp_time << ',' << product_name << ',' << lot_number << ',' << pnl_number << ',' << layer << ',' << job_ID << ',';
	for(vector<tag>::iterator iter=tags.begin(); iter!=tags.end(); ++iter)
	{
		if(iter->tag_ID == 56090)
		{
			if((iter->tag_content).x == 0)
				out_file << "off";
			else
				out_file << "on";
		}
		else if(iter->tag_ID == 56100)
		{
			if((iter->tag_content).x == 0)
				out_file << "off";
			else
				out_file << "on";
		}
		else if(iter->tag_ID == 56204)
		{
			if((iter->tag_content).x == 0)
				out_file << "Black";
			else
				out_file << "white";
		}
		else if(iter->tag_ID == 56211)
		{
			if((iter->tag_content).x == 0)
				out_file << "off";
			else
				out_file << "on";
		}
		else if(iter->tag_ID == 56213)
		{
			if((iter->tag_content).x == 1)
				out_file << "Circle";
			else
				out_file << "Ring";
		}
		else if(iter->tag_ID == 56218)
		{
			if(iter->tag_name == "Pre circle fitting on")
			{
				if((iter->tag_content).x == 0)
					out_file << "off";
				else
					out_file << "on";
			}
			else
			{
				if((iter->tag_content).x == 0)
					out_file << "Strong edge";
				else if((iter->tag_content).x == 1)
					out_file << "Inside edge";
				else if((iter->tag_content).x == 2)
					out_file << "Center edge";
				else
					out_file << "Outside edge";
			} 
		}
		else if(iter->tag_ID == 56502)
		{
			if((iter->tag_content).x == 0)
				out_file << "Single";
			else
				out_file << "Circle";
		}
		else if(iter->tag_ID == 56600)
		{
			if((iter->tag_content).x == 0)
				out_file << "Through-Hole";
			else if((iter->tag_content).x == 1)
				out_file << "Laser-Via";
			else
				out_file << "Backlight";
		}
		else if(iter->tag_ID == 56208)
		{
			if(iter->tag_name == "Template mask on")
			{
				if((iter->tag_content).x == 1)
					out_file << "on";
				else
					out_file << "off";
			}
			else
			{
				if(iter->type == Uint32_t)
					out_file << (iter->tag_content).x;
				else
					out_file << (iter->tag_content).y;
			}	
		}
		else
		{
			if(iter->type == Uint32_t)
				out_file << (iter->tag_content).x;
			else
				out_file << (iter->tag_content).y;
		}
		out_file << ',';
		//(iter + 1) == tags.end()? out_file << endl : out_file << ','; 
	}
	
	out_file << template_file_path << endl;
	out_file.close();		
}

template<class T, size_t N>
size_t array_size(T (&)[N])
{ 
	return N;
}


int main(){
	

	
	//scan files in the target folder
	DIR *dir;
	struct dirent *ent;
	string file_logshare, target_AFlog = "", exp_time, job_ID, product_name, layer, lot_number, pnl_number;
	string file_jobinfo, file_job, file_template;
	string template_ID;
	string up_logshare_path= "\\\\192.168.1.110\\logShare";
	string dn_logshare_path= "\\\\192.168.1.130\\logShare";
	string logshare_path = up_logshare_path;
	
	string up_jobinfo_path = "C:\\JobServer\\CommonFile\\JobInfo";
	string dn_jobinfo_path = "C:\\JobServer\\CommonFile\\JobInfo";
	string jobinfo_path = up_jobinfo_path;
	
	string up_jobs_path = "\\\\192.168.1.110\\logShare\\jobs";
	string dn_jobs_path = "\\\\192.168.1.130\\logShare\\jobs";
	string jobs_path = up_jobs_path;
	
	string up_template_path = "\\\\192.168.1.110\\fujifilm\\vecot\\mark\\usr";
	string dn_template_path = "\\\\192.168.1.130\\fujifilm\\vecot\\mark\\usr";
	string template_path = up_template_path;
	
	string up_template_backup_path = "C:\\Users\\fujifilm\\Desktop\\Fuji parser\\UP";
	string dn_template_backup_path = "C:\\Users\\fujifilm\\Desktop\\Fuji parser\\DN";
	string template_backup_path = up_template_backup_path;
	CreateDirectory((up_template_backup_path).c_str(), NULL);
	CreateDirectory((dn_template_backup_path).c_str(), NULL);
	
	string up_output_path = "C:\\Users\\fujifilm\\Desktop\\Fuji parser\\UP";
	string dn_output_path = "C:\\Users\\fujifilm\\Desktop\\Fuji parser\\DN";
	CreateDirectory((up_output_path).c_str(), NULL);
	CreateDirectory((dn_output_path).c_str(), NULL);
	
	string output_path = up_output_path;
	
	ifstream in_file;
	ofstream out_file;
	ofstream log_file;
	
	
	
	
	Side detect_side = down;	

	struct tm AFlog_t;
	time_t record_time = time(0);
	struct tm *now = localtime(&record_time);
	stringstream ss;
	ss  << now->tm_year + 1900
		<< now->tm_mon + 1
		<< now->tm_mday << "-"
		<< now->tm_hour
		<< now->tm_min
		<< now->tm_sec;
	string current_time = ss.str();
	log_file.open(("C:\\Users\\fujifilm\\Desktop\\Fuji parser\\" + current_time + "-log.txt").c_str(), ios_base::app);	
	log_file << "Current time: " << now->tm_year + 1900 << "-"
		<< now->tm_mon + 1 << "-"
		<< now->tm_mday << " "
		<< now->tm_hour << ":"
		<< now->tm_min << ":"
		<< now->tm_sec << endl;
	ShowWindow( GetConsoleWindow(), SW_HIDE );

	time_t read_time;
	while(true)
	{
		Sleep(0);
		if(detect_side == up)
		{
			detect_side = down;
			logshare_path = dn_logshare_path;
			jobinfo_path = dn_jobinfo_path;
			jobs_path = dn_jobs_path;
			template_path = dn_template_path;
			template_backup_path = dn_template_backup_path;
			output_path = dn_output_path;
		}
		else
		{
			detect_side = up;
			logshare_path = up_logshare_path;
			jobinfo_path = up_jobinfo_path;
			jobs_path = up_jobs_path;
			template_path = up_template_path;
			template_backup_path =  up_template_backup_path;
			output_path = up_output_path;
		}
		if ((dir = opendir (logshare_path.c_str())) != NULL)
		{
			//Find the "last" AF log file in logshare folder
			target_AFlog.clear();//Erase the content of target_AFlog
 	 		while ((ent = readdir (dir)) != NULL) 
			{
	    		file_logshare = ent->d_name;
				//Check whether the first two character in file name is "AF"
				if((file_logshare.substr(0, 2)).find("AF") != std::string::npos)
				{
					//Get time form file name
					AFlog_t.tm_year = atoi((file_logshare.substr(3, 4)).c_str()) - 1900;
					AFlog_t.tm_mon = atoi((file_logshare.substr(7, 2)).c_str()) - 1;
					AFlog_t.tm_mday = atoi((file_logshare.substr(9, 2)).c_str());
					AFlog_t.tm_hour = atoi((file_logshare.substr(12, 2)).c_str());
					AFlog_t.tm_min = atoi((file_logshare.substr(14, 2)).c_str());
					AFlog_t.tm_sec = atoi((file_logshare.substr(16, 2)).c_str());
					read_time = mktime(&AFlog_t);
					struct tm *read = localtime(&read_time);
					/*
					log_file << "Read time: " << read->tm_year + 1900 << "-"
						<< read->tm_mon + 1 << "-"
						<< read->tm_mday << " "
						<< read->tm_hour << ":"
						<< read->tm_min << ":"
						<< read->tm_sec << endl;*/				
					if(difftime(read_time, record_time) > 0)//We only deal with the job is being used after program starting since the template may be updated during exposeing and we can not make sure the past job used the same template as right now. 
					{
						target_AFlog = file_logshare;
						record_time = read_time;//To prevent from repeatly dealing with the same AF log
					}
				}
			}
		}
		else
		{	
 	 		log_file << "Can't open folder: " << logshare_path << endl;
 	 		closedir (dir);
			dir = 0, ent = 0;
 	 		continue;
	  		//return 1;
		}
		closedir (dir);
		dir = 0, ent = 0;
		//If there is no updated AF log, refresh the direcnt entities
		if(target_AFlog.empty())
			continue;
			
		log_file << "target AFlog: " << target_AFlog << endl;
		//log_file << file_logshare << endl;
	
		//After checking, we already get the last AF_log
		//Now, from the last AF_log file name, we extract the job_ID, panel number, and time
		size_t i = target_AFlog.find('_', 19);
		job_ID = target_AFlog.substr(19, i - 19);
		log_file << "job ID: " << job_ID << endl;
		pnl_number = target_AFlog.substr(i + 1, target_AFlog.find('.', (i + 1)) - (i + 1));
		log_file << "pnl number: " << pnl_number << endl;
		exp_time = target_AFlog.substr(3,8) + target_AFlog.substr(12,6);
		log_file << "exp time:" << exp_time << endl;
		
		//Get product number/lot number/layer from jobinfo
		bool found_Jobinfo = false;
		if ((dir = opendir (jobinfo_path.c_str())) != NULL)
		{
			//Find the file name composed with job_ID and .xml
			while ((ent = readdir (dir)) != NULL) 
			{
				file_jobinfo = ent->d_name;
				if(file_jobinfo == (job_ID + ".xml"))
				{
					found_Jobinfo = true;
					break;									
				}
			}
		}
		else
		{	
 	 		log_file << "Can't open folder: " <<jobinfo_path << endl;
 	 		closedir (dir);
			dir = 0, ent = 0;
 	 		continue;
	  		//return 1;
		}
		closedir (dir);
		dir = 0, ent = 0;
			//If the job_info did not found, the rest procces should be also skipped
		if(!found_Jobinfo)
		{
			log_file << "Job ID: " << job_ID << endl;
			log_file << "jobinfo not found" << endl;
			//system("pause");
			continue;
		}
		//log_file << job_ID << endl;
		log_file << "found job file: " << file_jobinfo << endl;
		//Copy the job info file before reading
		in_file.open((jobinfo_path+"\\"+file_jobinfo).c_str(), ios_base::binary);
		out_file.open((jobinfo_path+"\\temp_"+file_jobinfo).c_str(), ios_base::binary);
		out_file << in_file.rdbuf();
		in_file.close();
		out_file.close();
		log_file << "copy job file succeed" << endl;
		file_jobinfo = "temp_" + file_jobinfo;
		log_file << "temp file path: " << (jobinfo_path+"\\"+file_jobinfo) << endl;
		
		//Parse xml by tinyxml2
		tinyxml2::XMLDocument doc;
		doc.LoadFile((jobinfo_path+"\\"+file_jobinfo).c_str());
		
		tinyxml2::XMLElement *jobinfo=doc.RootElement();//Set the root node
		tinyxml2::XMLElement *controlinfo=jobinfo->FirstChildElement("ControlInfo");//Get the Child of root node named "ControlInfo"
		
		//Get product number
		tinyxml2::XMLElement *jobname=controlinfo->FirstChildElement("JobName1");
		product_name.clear();
		if(!(jobname == NULL) && !(jobname->GetText() == NULL))
			product_name = jobname->GetText();
		else
		{
			log_file << "Can't get product name" << endl;
			remove((jobinfo_path+"\\"+file_jobinfo).c_str());
			continue;
		}
		log_file << "product name: " << product_name << endl;
		
		//Get Layer
		layer.clear();
		jobname=controlinfo->FirstChildElement("JobName2");
		if(!(jobname == NULL) && !(jobname->GetText() == NULL))
			layer = jobname->GetText();
		else
		{
			log_file << "Can't get layer name" << endl;
			remove((jobinfo_path+"\\"+file_jobinfo).c_str());
			continue;
		}
		log_file << "layer: " << layer << endl;
			
		//Get lot number
		lot_number.clear();
		jobname=controlinfo->FirstChildElement("JobName4");
		if(!(jobname == NULL) && !(jobname->GetText() == NULL))
			lot_number = jobname->GetText();
		log_file << "lot number: " << lot_number << endl;
		
		remove((jobinfo_path+"\\"+file_jobinfo).c_str());		
	
		
		//Now we need to know which template it used (ID)
		//The template ID was recorded in the slot next to "JSA" and "ESA" in the csv file in job folder
		bool found_job = false;
		if ((dir = opendir (jobs_path.c_str())) != NULL)
		{
			//Ae before, find the csv file cantain job ID in its name
			while ((ent = readdir (dir)) != NULL) 
			{
				file_job = ent->d_name;
				if(file_job.find(job_ID + "-") == 0)//If only search the job_ID, it may find the wrong file when the date in file name contains the same number of job_ID. Maybe using job_ID+'-' would be more appropriate 
				{
					found_job = true;
					break;
				}
			}
		}
		else
		{	
 	 		log_file << "Can't open folder: " <<jobs_path << endl;
 	 		closedir (dir);
			dir = 0, ent = 0;
 	 		continue;
	  		//return 1;
		}
		closedir (dir);
		dir = 0, ent = 0;
		if(!found_job)
		{
			log_file << "Job : " << file_job << endl;
			log_file << "job not found." << endl;		
			//system("pause");
			continue;				
		}
		log_file << "found file in job folder: " << file_job << endl;
		
		//Copy the csv file before reading
		in_file.open((jobs_path+"\\"+file_job).c_str(), ios_base::binary);
		out_file.open((jobs_path+"\\temp_"+file_job).c_str(), ios_base::binary);
		out_file << in_file.rdbuf();
		in_file.close();
		out_file.close();
		log_file << "copy file complete" << endl;
		file_job = "temp_" + file_job;
		log_file << "temp file path: " << (jobs_path+"\\"+file_job) << endl;
		//We just check the slot next to "JSA"
		in_file.open((jobs_path+"\\"+file_job).c_str());
		string row;
		while(getline(in_file, row))
		{
			int i = row.find("JSA");
			if(i>0)
			{
				template_ID = row.substr(i+4, (row.substr(i+4).find(',')));
				template_ID = "ID_" + template_ID + ".tif";
				log_file << "template ID: " << template_ID << endl;
				break;
			}
		}				
		in_file.close();
		remove((jobs_path+"\\"+file_job).c_str());

	
		//We has collect all information we needed. Now we should focus on template.
		//Find the template file
		bool found_template = false;
		if ((dir = opendir (template_path.c_str())) != NULL)
		{
			while ((ent = readdir (dir)) != NULL) 
			{
				file_template = ent->d_name;
				//log_file << file_template << endl;
				if(file_template == template_ID)
				{
					found_template = true;
					break;
				}
			}	
		}
			else
		{	
 	 		log_file << "Can't open folder: " << template_path << endl;
 	 		closedir (dir);
			dir = 0, ent = 0;
 	 		continue;
	  		//return 1;
		}
		closedir (dir);
		dir = 0, ent = 0;
		if(!found_template)
		{
			log_file << "can't found template file" << endl;
			//system("pause");
			continue;
		}
		log_file << "found template file: " << file_template << endl;
		//Copy template
		ifstream source_tmp;
		source_tmp.open((template_path+"\\"+template_ID).c_str(), ios_base::binary);
		//Add some error checking if open file fail.
		ofstream dest_tmp;
		string dest_folder = product_name + "_" + lot_number + "_" + layer;//Create a folder named by product#, lot#, and layer#
		string dest_filename = exp_time + "_" + product_name + "_" + lot_number + "_" + layer + "_" + pnl_number + ".tif";//Create a file named by time, product#, lot#, layer#, and pnl#
		CreateDirectory((template_backup_path+"\\"+dest_folder).c_str(), NULL);//If the folder doesn't exist, create a new one
		dest_tmp.open((template_backup_path+"\\"+dest_folder+"\\"+ dest_filename).c_str(), ios_base::binary);
		dest_tmp << source_tmp.rdbuf();
		source_tmp.close();
		dest_tmp.close();
		
		//Extract the parameters in template
		vector<tag> tags;//The best way may be to create a array(or hash table) of length in 87(total tags number) to tell whether a tag should be recorded or not. If yes, the array will also tell you which element in tags you should record at.  
		int IDs[] = {56209, 56210, 56205, 56206, 56090, 56100, 56101, 56102, 56103, 56201, 56202, 56204, 56211, 56212, 56213, 56214, 56215, 56216, 56218, 56218, 56220, 56221, 56500, 56501, 56502, 56600, 56603, 56510, 56512, 56301, 56302, 56208, 56208, 56207};
		string names[] = {"Templae size(W)", "Template size(H)", "Center of gravity(X)", "Center of gravity(y)", "Apply prefilter to circlefit", "Level correct on", "Level correct min", "Level correct max", "Gamma", "Threshold High Res", "Threshold Low Res", "Mark color", "Circle fitting on", "M-est sigma", "Kind(Circle fit)", "Weak edge", "Strong edge", "Min-constrat", "Pre circle fitting on", "Edge detection", "Diameter(Circle fit:Circle)", "Margin(Circle fit:Circle)", "Mark num", "Mark num(threshold)", "Kind(Assemble)", "Strobo type", "Strobo control", "Diameter(Assemble:Circle)", "Margin(Assemble:Circle)", "Diameter(Circle fit:Ring)", "Margin(Circle fit:Ring)", "Template mask on", "Template offset", "Image offset"};
		int parameter_number = (int)array_size(IDs);
		tags.resize(parameter_number);
		for(int j=0; j<parameter_number; ++j)
		{
			(tags.at(j)).tag_ID = IDs[j];
			(tags.at(j)).tag_name = names[j];
		}
		in_file.open((template_backup_path+"\\"+dest_folder+"\\"+ dest_filename).c_str(), ios_base::binary);
		in_file.exceptions(ifstream::failbit | ifstream::badbit);
		//ios_base::binary is real important. If we don't do so, ifstream operation would try to analyse the file content and then we can't parse the file bits by bitss.
		
		//Check whether the double type uses the IEEE 754 format (as the same format in the tiff file)
		if(!(std::numeric_limits< double >::is_iec559))
		{
			log_file << "The double type is not in IEEE 754 format" << endl;
			//system("pause");
		}
		//Check the format
		char c1, c2;
		if(in_file.get(c1) && in_file.get(c2))
		{
			if(c1 == 'I' && c2 == 'I')
				log_file << "Littel endian" << endl;
			else if(c1 == 'M' && c2 == 'M')
			{
				log_file << "Big endian" << endl;
				system("pause");
			}
			else
			{
				log_file << "Undefined format" << endl; 
				system("pause");
			}
		}
		//Get the IFD offset
		in_file.seekg(4);
		char bytes[8];
		unsigned char u_bytes[8];
		uint32_t IFD_offset = 0;
		for(int j=0; j<4 ; ++j)
		{
			in_file.get(bytes[j]);
			u_bytes[j] = bytes[j];
		}
		memcpy(&IFD_offset, u_bytes, 4);
		log_file << "IFD offset is:" << IFD_offset << endl;
		
		//Get the tag number
		in_file.seekg(IFD_offset);
		for(int j=0; j<2 ; ++j)
		{
			in_file.get(bytes[j]);
			u_bytes[j] = bytes[j];
		}
		int tag_number = 0;
		memcpy(&tag_number, u_bytes, 2);
		log_file << "Tag number is:" << tag_number << endl;

		//Read each tags
		Initialize_tags(tags);
		
		uint32_t current_pos = IFD_offset + 2;
		in_file.seekg(current_pos);
		for(int j=0; j<tag_number; ++j)
		{
			log_file << current_pos << endl;
			int ID = 0;
			int type = 0;
			for(int k=0; k<2 ; ++k)
			{
			
				try
				{
					in_file.get(bytes[k]);
					u_bytes[k] = bytes[k];
				}
				catch(ifstream::failure e)
				{
					cerr << "Fail to read tag ID." << endl;
					system("pause");
				}
			}
			memcpy(&ID, u_bytes, 2);
			log_file << ID << endl;
			vector<tag>::iterator iter = tags.begin();
			for(; iter!=tags.end(); ++iter)
				if(iter->tag_ID == ID)
					break;
			if(iter != tags.end())
			{
				if(ID != 56218 && ID != 56208 && ID != 56207)
				{
					for(int k=0; k<2 ; ++k)
					{
						in_file.get(bytes[k]);
						u_bytes[k] = bytes[k];
					}
					memcpy(&type, u_bytes, 2);
					log_file << type << endl; 
					if(type == 1 || type == 4)
					{
						iter->type = Uint32_t;
						uint32_t content = 0;
						in_file.seekg(current_pos + 8);
						for(int k=0; k<4 ; ++k)
						{
							in_file.get(bytes[k]);
							u_bytes[k] = bytes[k];
						}
						if(ID == 56101 || ID == 56102)
							content += u_bytes[0];
						else
							memcpy(&content, u_bytes, 4);
						(iter->tag_content).x = content;
				
						log_file << ID << ":" << (iter->tag_content).x << endl;
					}
					else if(type == 12)
					{
						iter->type = Double;
						uint32_t offset =  0;
						double content = 0.0;
						in_file.seekg(current_pos + 8);
						for(int k=0; k<4 ; ++k)
						{
							in_file.get(bytes[k]);
							u_bytes[k] = bytes[k];
						}
						memcpy(&offset, u_bytes, 4);
				
						in_file.seekg(offset);
						
						for(int k=0; k<8 ; ++k)
						{
							in_file.get(bytes[k]);
							u_bytes[k] = bytes[k];
						}
				
						memcpy(&content, u_bytes, 8);
						(iter->tag_content).y = content;
						log_file << ID << ":" << (iter->tag_content).y << endl;	
					}
					else
					{
						log_file << "Tag ID:" << ID << "is not integer or double." << endl;
						system("pause"); 
					}	
				}
				else if(ID == 56218)
				{
					iter->type = Uint32_t;
					uint32_t content = 0;
					in_file.seekg(current_pos + 8);
					in_file.get(bytes[0]);
					content = bytes[0] >> 4;
					(iter->tag_content).x = content;
					log_file << ID << ":" << (iter->tag_content).x << endl;
			
					++iter;
					iter->type = Uint32_t;
					content = 0;
					content = bytes[0] & 15;
					(iter->tag_content).x = content;
					log_file << ID << ":" << (iter->tag_content).x << endl;
				}
				else if(ID == 56208)
				{
					iter->type = Uint32_t;
					(iter->tag_content).x = 1;
					
					++iter;
					iter->type = Uint32_t;
					uint32_t offset =  0;
					in_file.seekg(current_pos + 8);
					for(int k=0; k<4 ; ++k)
					{
						in_file.get(bytes[k]);
						u_bytes[k] = bytes[k];
					}
					memcpy(&offset, u_bytes, 4);
					(iter->tag_content).x = offset;
					//In fact, the data type of 56208 is an 8-bits byte which represent the gray scale of pixels and the content is an mask image with the same height and width as template image.
				}
				else//ID == 56207
				{
					iter->type = Uint32_t;
					uint32_t offset =  0;
					in_file.seekg(current_pos + 8);
					for(int k=0; k<4 ; ++k)
					{
						in_file.get(bytes[k]);
						u_bytes[k] = bytes[k];
					}
					memcpy(&offset, u_bytes, 4);
					(iter->tag_content).x = offset;
				}
			}
			current_pos = current_pos + 12;
			in_file.seekg(current_pos);
		}
		
		Output(tags, exp_time, product_name, lot_number, pnl_number, layer, job_ID, template_backup_path+"\\"+dest_folder+"\\"+ dest_filename, output_path+"\\"+dest_folder+"\\template_log.csv");
		in_file.close();
	//system("pause");
	//_sleep(10000);
	}
	log_file.close();
	return 0;
}
