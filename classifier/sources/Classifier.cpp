#include <iostream>
#include <fstream>
#include <string>
#include <cstring>//memcpy
#include <vector>
#define _USE_MATH_DEFINES //M_PI
#include <math.h>//pow, abs 
#include <typeinfo>
#include<cfloat>//DBL_EPSILON

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;

class img_par;
bool pre_circle_fit(Point2f&, const img_par&, const img_par&);
double circle_fit(Point2f&, vector<vector<Point>>&, const img_par&, const img_par&, const Mat&);
bool find_circle(Point2f&, double&, const img_par&, const img_par&, const Rect&);
bool gradient_matching(Point2f&, double&, const img_par&, const img_par&, const Rect&);
void radial_derivative(Point2f&, Mat&, const img_par&, const img_par&);
void constrained_circle_fit(Mat&, double, double, int&, double&, Mat&, int = 5);
void find_multi_max(Mat&, vector<Point2f>&, vector<vector<Point>>&, int&, double&, const img_par&, const img_par&, Mat&, Mat = Mat());
template <class T>
void non_max_suppression(Mat&, T, const string = "x");
void main_direction(vector<float>&, Size, Size);
Mat get_hogdescriptor_visual_image(const Mat&, const vector<float>&, Size, Size, int, double);//Code source: http://www.juergenbrauer.org/old_wiki/doku.php?id=public:hog_descriptor_computation_and_visualization
bool check_circularity(const double&, const double&, const RotatedRect&);
double mark_match(string, string, string);


class img_par{
public:
int img_height;
int img_width;
int template_height;//56210
int template_width;//56209
int mark_type;//56502
int black_white;//56204
int template_offset;//56207
int mask_offset;//56208
int level_correct_on;//56100
int level_correct_min;//56101
int level_correct_max;//56102
int mark_number_threshold;//56501
int circles_in_margin;
int mark_number;//56500
int pre_circle_fitting_on;//56218
int circle_fit_on;//56211

double center_x;//56205
double center_y;//56206
double single_diameter;//56220
double single_margin;//56221
double assembly_diameter;//56510
double assembly_margin;//56512
double high_res;//56201
double low_res;//56202
double gamma;//56103
double min_contrast;//56216

int tag_number;
uint32_t IFD_offset;

Mat img;
Mat templ;
Mat mask;

//default constructor
	img_par(){
	initialize_parameter();
	Mat img();
	Mat templ();
	Mat mask();	
	}

//truly used constructor: read image and parameters form template file
	img_par(string file_path){
	
	ifstream in_file;
	in_file.open(file_path.c_str(), ios_base::binary);
	//Because we use memcpy to get content, all parameters have to be initialized as 0.	
	initialize_parameter();
	
	//Get the IFD offset
	uint32_t current_pos = 4;
	IFD_offset = get_offset(in_file, current_pos);
	
	//Get the tag number
	get_content(in_file, IFD_offset, tag_number, 2);

	//Read each tags to initialize parameters
	current_pos = IFD_offset + 2;
	for(int j=0; j<tag_number; ++j)
	{
		int ID = 0;
		get_content(in_file, current_pos, ID, 2);
		
		if(ID == 56210)
			get_content(in_file, current_pos + 8, template_height, 4);
		else if(ID == 56209)
			get_content(in_file, current_pos + 8, template_width, 4);
		else if(ID == 56502)
			get_content(in_file, current_pos + 8, mark_type, 4);
		else if(ID == 56204)
			get_content(in_file, current_pos + 8, black_white, 4);
		else if(ID == 56207)
			get_content(in_file, current_pos + 8, template_offset, 4);
		else if(ID == 56208)
			get_content(in_file, current_pos + 8, mask_offset, 4);
		else if(ID == 56100)
			get_content(in_file, current_pos + 8, level_correct_on, 4);
		else if(ID == 56101)
			get_content(in_file, current_pos + 8, level_correct_min, 1);
		else if(ID == 56102)
			get_content(in_file, current_pos + 8, level_correct_max, 1);
		else if(ID == 56205)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, center_x, 8);
		}
		else if(ID == 56206)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, center_y, 8);
		}
		else if(ID == 56220)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, single_diameter, 8);
		}
		else if(ID == 56221)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, single_margin, 8);
		}
		else if(ID == 56510)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, assembly_diameter, 8);
		}
		else if(ID == 56512)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, assembly_margin, 8);
		}
		else if(ID == 56201)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, high_res, 8);
		}
		else if(ID == 56202)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, low_res, 8);
		}
		else if(ID == 56103)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, gamma, 8);
		}
		else if(ID == 56216)
		{
			uint32_t offset = get_offset(in_file, current_pos + 8);
			get_content(in_file, offset, min_contrast, 8);
		}
		else if(ID == 56501)
			get_content(in_file, current_pos + 8, mark_number_threshold, 4);
		else if(ID == 56500)
			get_content(in_file, current_pos + 8, mark_number, 4);
		else if(ID == 56218)
			get_content(in_file, current_pos + 8, pre_circle_fitting_on, 1, "4 bits");
		else if(ID == 56211)
			get_content(in_file, current_pos + 8, circle_fit_on, 4);
		
		current_pos = current_pos + 12;
	}

	//Read the image
	img = imread(file_path.c_str(), CV_LOAD_IMAGE_UNCHANGED);
	img_height = img.rows;
	img_width = img.cols;

	//Read the template
	templ = Mat(template_height, template_width, CV_8UC1, Scalar(0));
	read_image(in_file, templ, template_offset);

	//Read the mask
	mask = Mat(template_height, template_width, CV_8UC1, Scalar(1));
	read_image(in_file, mask, mask_offset);
	
	in_file.close();
	cout << "load image succeed:" << file_path << endl;
	}

	void initialize_parameter(){
	//Because we use memcpy to get content, all parameters have to be initialized as 0.	
	img_height = 0;
	img_width = 0;
	template_height = 0;//56210
	template_width = 0;//56209
	mark_type = 0;//56502
	black_white = 0;//56204
	template_offset = 0;//56207
	mask_offset = 0;//56208
	level_correct_on = 0;//56100
	level_correct_min = 0;//56101
	level_correct_max = 0;//56102
	mark_number_threshold = 0;
	circles_in_margin = 0;
	mark_number = 0;//56500
	pre_circle_fitting_on = 0;//56218
	circle_fit_on = 0;//56211
	
	center_x = 0.0;//56205
	center_y = 0.0;//56206
	single_diameter = 0.0;//56220
	single_margin = 0.0;//56221
	assembly_diameter = 0.0;//56510
	assembly_margin = 0.0;//56512
	high_res = 0.0;//56201
	low_res = 0.0;//56202
	gamma = 0.0;//56103
	min_contrast = 0.0;
	
	tag_number = 0;
	IFD_offset = 0;
	}
		
	uint32_t get_offset(ifstream& in_file, uint32_t current_pos)
	{
		char bytes[8];
		unsigned char u_bytes[8];
		int content_size = 4;
		uint32_t offset = 0;
		in_file.seekg(current_pos);
		for(int j=0; j<content_size ; ++j)
		{
			in_file.get(bytes[j]);
			u_bytes[j] = bytes[j];
		}
		memcpy(&offset, u_bytes, content_size);
		return offset;
	}
	
	template <class T>
	void get_content(ifstream& in_file, uint32_t current_pos, T& content, const int& content_size, const string flag = "int_or_double")
	{
		char bytes[8];
		unsigned char u_bytes[8];
		in_file.seekg(current_pos);
		for(int j=0; j<content_size ; ++j)
		{
			in_file.get(bytes[j]);
			u_bytes[j] = bytes[j];
		}
		if(flag == "4 bits")
			content = bytes[0] >> 4;
		else
		memcpy(&content, u_bytes, content_size);
	}
	
	void read_image(ifstream& in_file, Mat& image, int& offset)
	{
		if(offset != 0)
		{
			char bytes[1];
			unsigned char u_bytes[1];
			in_file.seekg(offset);	
			for(int i=0; i<image.size().height*image.size().width; ++i)
			{
				in_file.get(bytes[0]);
				u_bytes[0] = bytes[0];
				int row = i / image.size().width;
				int col = i % image.size().width;
				image.at<uchar>(row, col) = u_bytes[0];  
			}
		}
	}

};

template <class T>
void non_max_suppression(Mat& img, T, const string dir_flag)
{
	//Remove the edges point which gradient is not a local maximum.
	if(dir_flag != "x")
		transpose(img, img);
	for(int i=0; i<img.rows; ++i)
	{
		float last_element = img.at<T>(i, 0);
		for(int j=1; j<img.cols; ++j)
		{
			if(img.at<T>(i, j) > last_element && (last_element = img.at<T>(i, j)))
				img.at<T>(i, j-1) = 0;
			else
				img.at<T>(i, j) = 0;		
		}
	}
	if(dir_flag != "x")
		transpose(img, img);
	
	cout << "circle fit: non-max suppression succeed." << endl;
}

void main_direction(vector<float>& descriptorValues, Size winSize, Size cellSize)
{
	int blocks_in_x_dir = winSize.width / cellSize.width - 1;
	int blocks_in_y_dir = winSize.height / cellSize.height - 1;
	int gradientBinSize = 9;
	int descriptorDataIdx = 0;
	for(int xblock=0; xblock<blocks_in_x_dir; ++xblock)
	{
		for(int yblock=0; yblock<blocks_in_y_dir; ++yblock)
		{
			for(int cellNr=0; cellNr<4; ++cellNr)
			{
				float maxbinVal = 0;
				int startIdx = descriptorDataIdx;
				for(int binIdx=0; binIdx<gradientBinSize; ++binIdx)
				{	
					if(descriptorValues[descriptorDataIdx] > maxbinVal)
						maxbinVal = descriptorValues[descriptorDataIdx];
					++descriptorDataIdx;
				}
				for(int binIdx=0; binIdx<gradientBinSize; ++binIdx)
				{
					if(descriptorValues[startIdx] == maxbinVal)
						descriptorValues[startIdx] = maxbinVal;
					else
						descriptorValues[startIdx] = 0;
					++startIdx;
				}
			}
		}	
	}
}

Mat get_hogdescriptor_visual_image(const Mat& origImg, const vector<float>& descriptorValues, Size winSize, Size cellSize, int scaleFactor, double viz_factor)
{   
    Mat visual_image;
    resize(origImg, visual_image, Size(origImg.cols*scaleFactor, origImg.rows*scaleFactor));
 	cvtColor(visual_image, visual_image, CV_GRAY2BGR);
 	
    int gradientBinSize = 9;
    // dividing 180¢X into 9 bins, how large (in rad) is one bin?
    float radRangeForOneBin = 2*M_PI/(float)gradientBinSize;//2pi for signed gradient?
 
    // prepare data structure: 9 orientation / gradient strenghts for each cell
	int cells_in_x_dir = winSize.width / cellSize.width;
    int cells_in_y_dir = winSize.height / cellSize.height;
    int totalnrofcells = cells_in_x_dir * cells_in_y_dir;
    float*** gradientStrengths = new float**[cells_in_y_dir];
    int** cellUpdateCounter   = new int*[cells_in_y_dir];
    for (int y=0; y<cells_in_y_dir; y++)
    {
        gradientStrengths[y] = new float*[cells_in_x_dir];
        cellUpdateCounter[y] = new int[cells_in_x_dir];
        for (int x=0; x<cells_in_x_dir; x++)
        {
            gradientStrengths[y][x] = new float[gradientBinSize];
            cellUpdateCounter[y][x] = 0;
 
            for (int bin=0; bin<gradientBinSize; bin++)
                gradientStrengths[y][x][bin] = 0.0;
        }
    }
 
    // since there is a new block on each cell (overlapping blocks!) but the last one
    int blocks_in_x_dir = cells_in_x_dir - 1;//Limit the block size & black stride
    int blocks_in_y_dir = cells_in_y_dir - 1;
 
    // compute gradient strengths per cell
    int descriptorDataIdx = 0;
    int cellx = 0;
    int celly = 0;
 
    for (int blockx=0; blockx<blocks_in_x_dir; blockx++)
    {
        for (int blocky=0; blocky<blocks_in_y_dir; blocky++)            
        {
            // 4 cells per block ...
            for (int cellNr=0; cellNr<4; cellNr++)
            {
                // compute corresponding cell nr
                int cellx = blockx;
                int celly = blocky;
                if (cellNr==1) celly++;
                if (cellNr==2) cellx++;
                if (cellNr==3)
                {
                    cellx++;
                    celly++;
                }
 
                for (int bin=0; bin<gradientBinSize; bin++)
                {
                    float gradientStrength = descriptorValues[ descriptorDataIdx ];
                    descriptorDataIdx++;
 
                    gradientStrengths[celly][cellx][bin] += gradientStrength;
 
                } // for (all bins)
 
 
                // note: overlapping blocks lead to multiple updates of this sum!
                // we therefore keep track how often a cell was updated,
                // to compute average gradient strengths
                cellUpdateCounter[celly][cellx]++;
 
            } // for (all cells)
 
 
        } // for (all block x pos)
    } // for (all block y pos)
 
 
    // compute average gradient strengths
    for (int celly=0; celly<cells_in_y_dir; celly++)
    {
        for (int cellx=0; cellx<cells_in_x_dir; cellx++)
        {
 
            float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];
 
            // compute average gradient strenghts for each gradient bin direction
            for (int bin=0; bin<gradientBinSize; bin++)
            {
                gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
            }
        }
    }
 
    // draw cells
    for (int celly=0; celly<cells_in_y_dir; celly++)
    {
        for (int cellx=0; cellx<cells_in_x_dir; cellx++)
        {
            int drawX = cellx * cellSize.width;
            int drawY = celly * cellSize.height;
 
            int mx = drawX + cellSize.width/2;
            int my = drawY + cellSize.height/2;
 
            rectangle(visual_image,
                      Point(drawX*scaleFactor,drawY*scaleFactor),
                      Point((drawX+cellSize.width)*scaleFactor,
                      (drawY+cellSize.height)*scaleFactor),
                      CV_RGB(100,100,100),
                      1);
 
            // draw in each cell all 9 gradient strengths
            for (int bin=0; bin<gradientBinSize; bin++)
            {
                float currentGradStrength = gradientStrengths[celly][cellx][bin];
 
                // no line to draw?
                if((currentGradStrength - 0) < FLT_EPSILON)
                    continue;
 				//direction
                float currRad = bin * radRangeForOneBin + radRangeForOneBin/2;
 
                float dirVecX = cos( currRad );
                float dirVecY = sin( currRad );
                float maxVecLen = cellSize.width/2;
                float scale = viz_factor; // just a visual_imagealization scale,
                                          // to see the lines better
 
                // compute line coordinates
                float x1 = mx;
                float y1 = my;
                float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
                float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;
 
                // draw gradient visual_imagealization
                arrowedLine(visual_image,
                     Point(x1*scaleFactor,y1*scaleFactor),
                     Point(x2*scaleFactor,y2*scaleFactor),
                     CV_RGB(255,0,0),
                     1);
 
            } // for (all bins)
 
        } // for (cellx)
    } // for (celly)
 
 
    // don't forget to free memory allocated by helper data structures!
    for (int y=0; y<cells_in_y_dir; y++)
    {
      for (int x=0; x<cells_in_x_dir; x++)
      {
           delete[] gradientStrengths[y][x];            
      }
      delete[] gradientStrengths[y];
      delete[] cellUpdateCounter[y];
    }
    delete[] gradientStrengths;
    delete[] cellUpdateCounter;
 
    return visual_image;
 
}

void gradient_matchTemplate(const vector<float>& img_gradient, const vector<float>& templ_gradient, Size winSize, Size cellSize, Mat& match_result)
{
	int blocks_in_x_dir = winSize.width / cellSize.width - 1;
	int blocks_in_y_dir = winSize.height / cellSize.height - 1;
	int cells_in_block = 4;
	int gradientBinSize = 9;
	int descriptorDataIdx = 0;
	Mat templ(Size(blocks_in_x_dir, blocks_in_y_dir), CV_32FC1, Scalar(0));
	Mat image(Size(blocks_in_x_dir, blocks_in_y_dir), CV_32FC1, Scalar(0));
	
	//Pad the image size to image.size+2*template.size
	copyMakeBorder(image, image, templ.cols, templ.cols, templ.rows, templ.rows, BORDER_CONSTANT, Scalar(0));
	
	//Initialize match result
	match_result = Mat(image.rows-templ.rows+1, image.cols-templ.cols+1, CV_32FC1, Scalar(0));

	for(int cellNr=0; cellNr<cells_in_block; ++cellNr)
	{
		for(int bin=0; bin<gradientBinSize; ++bin)
		{
			//Make image & template Mat
			for(int row=0; row<blocks_in_x_dir; ++row)
			{
				for(int col=0; col<blocks_in_y_dir; ++col)
				{
					image.at<float>(templ.rows+row, templ.cols+col) = img_gradient.at((col*blocks_in_x_dir+row)*cells_in_block*gradientBinSize+cellNr*gradientBinSize+bin);
					templ.at<float>(row, col) = templ_gradient.at((col*blocks_in_x_dir+row)*cells_in_block*gradientBinSize+cellNr*gradientBinSize+bin);
				}
			}
			
			//Accumulate match result
			Mat cell_match(match_result.rows, match_result.cols, CV_32FC1, Scalar(0));
			matchTemplate(image, templ, cell_match, CV_TM_CCORR);
			match_result = match_result + cell_match;
		}
	}	
}

bool find_circle(Point2f& fitted_loc, double& circle_diameter, const img_par& templ, const img_par& img, const Rect& roi)
{
	//Get Otsu's threshold for Canny detector
	Mat thresholding_region(img.img, roi);//thresholding_img is a subregion of imgray. This means that if modify the former, it also modify the latter.
	thresholding_region = thresholding_region.clone();//Create a new space to store thresholding_img for further operation.
	
	//Blur image in the same way to get correct Otsu's threshold.
	Mat dst;
	bilateralFilter(thresholding_region, dst, 9, 150, 150);
	thresholding_region = dst.clone();
	
	Mat dx, dy;
	Sobel(thresholding_region, dx, CV_32FC1, 1, 0);
	Sobel(thresholding_region, dy, CV_32FC1, 0, 1);
	
	//non maximum suppression before thresholding;
	non_max_suppression(dx, float(0));
	non_max_suppression(dy, float(0), "y");
	
	//Convert Sobel image into CV_8UC1 type and get Otsu's threshold by find the minimun value other than zero after thresholding with CV_THRESH_OTSU.
	Mat dx_dy = abs(dx) + abs(dy);//Calculate the derivative magnitude in the same way as Canny detector.
	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(dx_dy, &minVal, &maxVal, &minLoc, &maxLoc);
	double alpha = 255/maxVal;
	Mat uchar_dx_dy;
	dx_dy.convertTo(uchar_dx_dy, CV_8UC1, alpha);
	threshold(uchar_dx_dy, uchar_dx_dy, 0, 255, THRESH_TOZERO | CV_THRESH_OTSU);
	minMaxLoc(uchar_dx_dy, &minVal, &maxVal, &minLoc, &maxLoc, uchar_dx_dy);//Use thresholding image with threshlod-type: THRESH_TOZERO as mask to find the minimum value other than zero.
	double otsu_threshold = minVal/alpha;

	//If the otsu_threshold of sobel image lower than an empirical value, which is 100 now, judge the circle does not exist.
	if(otsu_threshold < 10)
	{
		cout << "pre-circle fit: otsu's theshold is less than 10." << endl;
		return false;
	}
		
	//Reduce noise
	//The dst Mat of bilateralFilter can not be the src Mat.
	bilateralFilter(img.img, dst, 9, 150, 150);//Use bilateralFilter which could preserve egde strength and also reduce noise.
	dst = dst.clone();
	
	//Apply the Hough Transform to find the circles
	//Apply Canny detector on image to merely send edges into HoughCircles for preventing HoughCircles crash.
	Canny(dst, dst, otsu_threshold/2, otsu_threshold);
	Mat matching_region(img.img.size(), CV_8UC1, Scalar(0));
	rectangle(matching_region, roi, 1, CV_FILLED);
	matching_region = dst.mul(matching_region);

	if(countNonZero(matching_region) == 0)
	{
		cout << "pre-circle fit: Can not find any edges after applying Canny method." << endl;
		return false;
	}
	vector<Vec3f> circles;
	HoughCircles(matching_region, circles, CV_HOUGH_GRADIENT, 1, 1, 1, 1, templ.single_diameter/2-templ.single_margin, templ.single_diameter/2+templ.single_margin);//Merely set the thresholds to 1 to prevent crash.
	if(circles.size() == 0)
	{	cout << "pre-circle fit: HoughCircles fail." << endl;
		return false;
	}
	
	//If the circle's center is out of templ.single_diameter/2 from template's center, judge a fail circle was returned.
	Point v1(circles[0][0], circles[0][1]);
	Point v2(fitted_loc.x+templ.center_x, fitted_loc.y+templ.center_y);
	if(norm(v1-v2) < templ.single_diameter/2)
	{
		fitted_loc.x = min(max(circles[0][0] - templ.center_x, 0.0), (double)img.img.cols);//Thre fitted_loc represent the upper_left corner of the template when template matching.
		fitted_loc.y = min(max(circles[0][1] - templ.center_y, 0.0), (double)img.img.rows);
		circle_diameter = circles[0][2] * 2;
	}
	else
	{
		cout << "pre-circle fit: bias fitted-circle center." << endl;
		return false;
	}
	cout << "pre-circle fit succeed." << endl;
	return true;
}

bool gradient_matching(Point2f& fitted_loc, double& circle_diameter, const img_par& templ, const img_par& img, const Rect& roi)
{
	Mat matching_region(img.img.size(), CV_8UC1, Scalar(0));
	rectangle(matching_region, roi, Scalar(1), CV_FILLED);
	
	//Crop image and resize to 128*128 
	Mat dst;
	bilateralFilter(img.img, dst, 9, 150, 150);
	dst = Mat(dst, roi);
	dst = dst.clone();
	double resize_scale = 64.0 / dst.cols;
	resize(dst, dst, Size(64, 64), 0, 0, CV_INTER_CUBIC);
	
	//Set HOG parameters.
	HOGDescriptor hog(
        Size(64,64), //winSize
        Size(16,16), //blocksize
        Size(8,8), //blockStride,
        Size(8,8), //cellSize,
        9, //nbins,
        1, //derivAper,
        -1, //winSigma,
        0, //histogramNormType,
        0.2, //L2HysThresh,
        -1,//gammal correction,
        64,//nlevels=64
        1);//Use signed gradients 
	
	//Get gradients
	vector<float> img_descriptor;
	hog.compute(dst, img_descriptor);
	//Visualize gradient for each cells
	Mat img_gradient = get_hogdescriptor_visual_image(dst, //gray image
														 img_descriptor, //HOGdescriptor
														 Size(64, 64), //winSize 
														 Size(8, 8), //cellSize
														 10, //resize scale of output image
														 2.5);//Magnifying scale of line length representing the gradient strength 
	
	//Get main direction and normalize strength to 1
	main_direction(img_descriptor, //HOGdescriptor
					 Size(64, 64), //winSize
					 Size(8, 8)); //cellSize

	for(int i=0; i<1+templ.single_margin*2; ++i)
	{ 
		circle_diameter = (templ.single_diameter + pow(-1,i%2)*i) * resize_scale; //templ.single_diameter or img.single_diameter??
		if(circle_diameter <= 0)
			continue;
		//Make circle template
		Mat circle_templ(Size(64, 64), CV_8UC1, Scalar(0));
		circle(circle_templ, Point(circle_templ.cols/2, circle_templ.rows/2), circle_diameter/2, Scalar(255), CV_FILLED);
		if(templ.black_white == 0)//black mark
			threshold(circle_templ, circle_templ, 0, 255, THRESH_BINARY_INV);
		
		//Get gradient template
		vector<float> templ_descriptor;
		hog.compute(circle_templ, templ_descriptor);		
		Mat templ_gradient = get_hogdescriptor_visual_image(circle_templ, templ_descriptor, Size(64, 64), Size(8, 8), 10, 2.5);
			
		//Get main direction and normalize strength to 1
		main_direction(templ_descriptor, Size(64, 64), Size(8, 8)); 
		
		//Template matching
		Mat match_result;
		gradient_matchTemplate(img_descriptor, //image HOGdescriptor
							 templ_descriptor, //template HOGdescriptor
							 Size(64, 64), //winSize 
							 Size(8, 8), //cellSize
							 match_result); //Match result
		
		double  minVal, maxVal;
		Point minLoc, maxLoc;
		minMaxLoc(match_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
		if(maxVal >= 8)//An empirical value.
		{
			cout << "pre-circle fit: gradient matching succeed." << endl;
			return true;
		}
	}
	cout << "pre-circle fit: gradient matching fail." << endl;
	return false;
}

void radial_derivative(Point2f& fitted_loc, Mat& r_der, const img_par& templ, const img_par& img)
{
	//Get polar image
	Mat polar_img;
	Point center(fitted_loc.x+templ.center_x, fitted_loc.y+templ.center_y);//Since the image has become broader, the max value location does not need to subtract template_height/2 or templeate_width/2 
	double r = templ.single_diameter/2+templ.single_margin;
	linearPolar(img.img, polar_img, center, r, INTER_LINEAR + WARP_FILL_OUTLIERS);
	
	//Scharr filter
	Scharr(polar_img, r_der, CV_32FC1, 1, 0);
	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(r_der, &minVal, &maxVal, &minLoc, &maxLoc);
	
	//If it's the white mark, inverse the plus-minus sign to make sure the desired edges would have positive gradient.
	if(templ.black_white == 1)//white mark
		r_der = -r_der;
	
	//Remove the un-desired edges which should has the negative gradient.
	threshold(r_der, r_der, 0, maxVal, THRESH_TOZERO);
	//Screen the edges out of the inner margin.
	Point p1(0, 0), p2((templ.single_diameter/2-templ.single_margin)/(templ.single_diameter/2+templ.single_margin)*r_der.cols, r_der.rows);
	rectangle(r_der, p1, p2, Scalar(0), CV_FILLED);

	//non-maximum suppression
	non_max_suppression(r_der, float(0));
	
	linearPolar(polar_img, polar_img, center, r, WARP_INVERSE_MAP + INTER_LINEAR + WARP_FILL_OUTLIERS);
	linearPolar(r_der, r_der, center, r, WARP_INVERSE_MAP + INTER_LINEAR + WARP_FILL_OUTLIERS);//The pixel's intensity may changed because interpolation during polar to cartesian transformation.
	
	//filter out the edges with gradient lower than min constrast
	threshold(r_der, r_der, templ.min_contrast, 255, THRESH_TOZERO);
	
	//Get the strong edge by Otsu's threshold
	r_der.convertTo(r_der, CV_8UC1, 255/maxVal);//The Otsu's threshold can only apply to Mat with CV_8UC1 type and the 255/maxVal coefficient preserve the relative intensity between each pixels.
	threshold(r_der, r_der, 0, 255, THRESH_BINARY | CV_THRESH_OTSU);
	//destroyWindow("Strong edges");
	cout << "circle fit: radial derivative succeed." << endl;
}

void constrained_circle_fit(Mat& edge_points, double constrained_diameter, double constrained_margin, int& fitted_points, double& fitted_diameter, Mat& fitted_circle, int line_width)
{
	vector<double> match_scores;//Create the vector to stores the matching score for each diameters.
	int round_margin = cvRound(constrained_margin);
	match_scores.resize(1+2*round_margin);
	vector<Point> circle_centers;//Create the vector to stores the matching location for each diameters.
	circle_centers.resize(1+2*round_margin);
	int best_fit_circle = 0;
	double best_fit_diameter = 0;
	double max_score = 0;
	//do template-matching with circle templates in differenet sizes. 
	for(int i=0; i<match_scores.size(); ++i)
	{
		double circle_diameter = constrained_diameter+pow(-1,i%2)*i;//The circle template would draw the circles with diameter in constrained_diameter, constrained_diameter-1, constrained_diameter+1, constrained_diameter-2, constrained_diameter+2 subsequently.
		if(circle_diameter <= 0)
			continue;
		Mat circle_templ(min(circle_diameter+line_width+1, (double)edge_points.rows), min(circle_diameter+line_width+1, (double)edge_points.cols), CV_8UC1, Scalar(0));
		Point c(circle_templ.cols/2, circle_templ.rows/2);
		circle(circle_templ, c, circle_diameter/2, Scalar(255), line_width);//Point(x, y)

		Mat match_result(edge_points.rows-circle_templ.rows+1, edge_points.cols-circle_templ.cols+1, CV_64FC1, Scalar(0.0));
		matchTemplate(edge_points, circle_templ, match_result, CV_TM_CCORR);
		double minVal, maxVal;
		Point minLoc, maxLoc;
		minMaxLoc(match_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
		match_scores.at(i) = maxVal;

		circle_centers.at(i) = maxLoc;
		if(maxVal > max_score)//Only update the best-fit circle when meet a higher score, which would gaurantee the diameter closest to constrained_diameter would be preserved if there are two or more diameters having the same score.
		{
			max_score = maxVal;
			best_fit_circle = i;
			best_fit_diameter = circle_diameter;
		}
	}
	fitted_diameter = best_fit_diameter;
	fitted_points = max_score/255/255;
	
	//Return a Mat with the best fit circle at the max score location.
	fitted_circle = Mat(edge_points.size(), CV_8UC1, Scalar(0));
	Point c = circle_centers.at(best_fit_circle);
	c.x = c.x + (best_fit_diameter+6)/2;
	c.y = c.y + (best_fit_diameter+6)/2; 
	circle(fitted_circle, c, best_fit_diameter/2, Scalar(255), line_width);
	
	//Preserve the points in the best-fit circle template.		
	edge_points = edge_points.mul(fitted_circle);
}

bool pre_circle_fit(Point2f& fitted_loc, const img_par& templ, const img_par& img)
{
	Point v(min(fitted_loc.x+templ.template_width, (float)img.img.cols), min(fitted_loc.y+templ.template_height, (float)img.img.rows));//If the edges of roi are out of img.img's border, it will throw exception.
	Rect roi(fitted_loc, v);
	double circle_diameter = 0;
	if(!find_circle(fitted_loc, circle_diameter, templ, img, roi))
	{
		return false;
	}
	//The operations of max and min below are to prevent the point going beyond the border of image and also make sure the shape of roi is a square.
	//If any of x and y coordinate falls outside the border, set the minimun distance form fitted_loc to border as the edge size of the square.
	v = Point(min(min(fitted_loc.x+max(templ.template_width, templ.template_height), (float)img.img.cols), fitted_loc.x+img.img.rows-fitted_loc.y), min(min(fitted_loc.y+max(templ.template_width, templ.template_height), (float)img.img.rows), fitted_loc.y+img.img.cols-fitted_loc.x));
	roi = Rect(fitted_loc, v);
	if(!gradient_matching(fitted_loc, circle_diameter, templ, img, roi))
	{
		return false;
	}
	cout << "pre-circle fit succeed." << endl;
	return true;	
}

double circle_fit(Point2f& fitted_loc, vector<vector<Point>>& edges, const img_par& templ, const img_par& img, const Mat& match_result)
{
	//Pre-circle fit
	//if(templ.pre_circle_fitting_on == 1)
	if(1)
	{
		if(!pre_circle_fit(fitted_loc, templ, img))
			return 0;
	}
	
	//Extract circle edge
	Mat r_der;
	radial_derivative(fitted_loc, r_der, templ, img);
	
	//Constrained circle fit
	double fitted_diameter = 0;
	int fitted_edges = 0;
	Mat fitted_circle;
	constrained_circle_fit(r_der, templ.single_diameter, templ.single_margin, fitted_edges, fitted_diameter, fitted_circle, 5);//The intensity of r_der has to be 255 because the constrained_circle_fit calculate the fitted number by score/255/255.
	//Calculate the matching score by the ratio of fit-edges to the edge points of circle template.
	double fit_score = (double)fitted_edges/(double)countNonZero(fitted_circle);
	//Only compare the number of fitted points rather than fitted fraction with a empirical number because the high occurrence rate of bad circularity.
	if(fitted_edges < 7)
	{
		cout << "circle fit: fitted edges are less than 10." << endl;
		return 0;
	}
	
	//Correct the fitted location
	r_der = r_der.mul(fitted_circle);
	vector<vector<Point>> contours;
	findContours(r_der, contours, RETR_LIST, CHAIN_APPROX_NONE);
	vector<Point> all_contours;
	for(size_t i=0; i<contours.size(); ++i)
	{
		for(size_t j=0; j<contours[i].size(); ++j)
			all_contours.push_back(contours[i][j]);				
	}
	//Check circularity
	RotatedRect fit_circle = fitEllipse(Mat(all_contours));
	if(!check_circularity(fitted_diameter, templ.single_margin, fit_circle))
	{
		cout << "circle fit fail." << endl;
		return 0;
	}
	if((fit_circle.center.x - templ.center_x) < 0 || (fit_circle.center.x - templ.center_x) >= match_result.cols || (fit_circle.center.y - templ.center_y) < 0 || (fit_circle.center.y - templ.center_y) >= match_result.rows)
	{
		cout << "circle fit: circle's center is beyond the border." << endl;
		return 0;
	}
	//It may find that the center corrected after fitEllipse was not better than HoughCircle in pre_circle_fit. It was because the ellipse return by fitEllipse may not be the desired size. However, we have no reason for not using it until finding way to extract more complete edges.  
	fitted_loc.x = fit_circle.center.x - templ.center_x;//The fitted_loc represent the upper_left corner of the template when template matching.
	fitted_loc.y = fit_circle.center.y - templ.center_y;
	edges.push_back(all_contours);
	if(fit_score > 0)
	{	
		cout << "circle fit succeed." << endl;	
		return fit_score;
	}
	else
	{
		cout << "circle fit fail." << endl;
		return 0;
	}
}

void find_multi_max(Mat& match_result, vector<Point2f>& circles_location, vector<vector<Point>>& edges, int& circle_number, double& fitted_diameter, const img_par& templ, const img_par& img, Mat& fitted_circle, Mat minMax_mask)
{
	//Create a Mat to store the circles location.
	Mat circles(match_result.size(), CV_8UC1, Scalar(0));
	for(int i=0; i<circles_location.size(); ++i)
		circles.at<uchar>(circles_location.at(i)) = 255;

	while(circle_number < templ.mark_number)
	{
		double minVal, maxVal;
		Point minLoc, maxLoc;
		minMaxLoc(match_result, &minVal, &maxVal, &minLoc, &maxLoc, minMax_mask);
		if(maxVal < templ.high_res)
			break;
		Point2f correct_maxLoc(maxLoc);
		cout << "checking location:" << correct_maxLoc << endl;
		if(templ.circle_fit_on == 1)
		{
			if((circle_fit(correct_maxLoc, edges, templ, img, match_result) - 0) < DBL_EPSILON)
			{
				circle(match_result, maxLoc, (int)(templ.single_diameter+1)/2, Scalar(0.0), CV_FILLED);
				continue;
			}
		}
		++circle_number;
		circles.at<uchar>(correct_maxLoc) = 255;
		circles_location.push_back(correct_maxLoc);
		
		if((templ.single_diameter - 0) < DBL_EPSILON)//Fill a circle area as the same size of template circle
			circle(match_result, correct_maxLoc, (int)(templ.single_diameter+1)/2, Scalar(0.0), CV_FILLED);
		else//Fill a square range as the same size of template image
		{
			Point v1(0, 0), v2(match_result.cols, match_result.rows);
			double d;
			if((d = (correct_maxLoc.x - templ.template_width/2)) > 0)
				v1.x = (int)d;
			if((d = (correct_maxLoc.x + templ.template_width/2)) < (double)circles.rows)
				v2.x = (int)d;
			if((d = (correct_maxLoc.y - templ.template_height/2)) > 0)
				v1.y = (int)d;
			if((d = (correct_maxLoc.y + templ.template_height/2)) < (double)circles.cols)
				v2.y = (int)d;
			rectangle(match_result, v1, v2, Scalar(0.0), CV_FILLED);
		}
	}
	
	Mat _result;
	cvtColor(img.img, _result, CV_GRAY2BGR);
	
	constrained_circle_fit(circles, templ.assembly_diameter, templ.assembly_margin, circle_number, fitted_diameter, fitted_circle);
	
	int i = 0;
	for(vector<Point2f>::iterator iter=circles_location.begin(); iter!=circles_location.end(); ++iter)
	{
		if(circles.at<uchar>(*iter) == 0)
		{
			circles_location.erase(iter);
			if(templ.circle_fit_on == 1)
			{
				vector<vector<Point>>::iterator e_iter = edges.begin();
				e_iter = e_iter + i;
				edges.erase(e_iter);//In fact, this can be rewritten by "iter = edges.erase(e_iter)" because erase() returns the element followed removed element.
			}
			iter = circles_location.begin();//This line can be removed.
			if(iter == circles_location.end())
				break;
			i = 0;
		}
		++i;
	}
}

bool check_circularity(const double& target_diameter, const double& target_margin, const RotatedRect& fitted_ellipse)
{
	//The criteria of difference between fitted size to template size has been loosed to 2*target_magin
	if(abs(fitted_ellipse.size.height-target_diameter)<=2*target_margin && abs(fitted_ellipse.size.width-target_diameter)<=2*target_margin)
		return true;
	else
	{
		cout << "bad circularity." << endl;
		return false;
	}
}

double mark_match(string img_path, string templ_path, string result_path)
{
	if(!(std::numeric_limits< double >::is_iec559))
	{
		cout << "The double type is not in IEEE 754 format" << endl;
		system("pause");
	}
	
	img_par img(img_path);//img is the image to be compared.
	img_par templ(templ_path);//templ is the image comparing to img as an template
	
	//Compare mark type
	if(img.mark_type != templ.mark_type)
	{
		cout << "mark type different." << endl;
		return 0;
	}
	//Compare mark color
	if(img.black_white != templ.black_white)
	{
		cout << "mark color different." << endl;
		return 0;
	}
	
	//Compare single size
	if(img.single_diameter > (templ.single_diameter + 2*templ.single_margin) || img.single_diameter < (templ.single_diameter - 2*templ.single_margin))
	{
		cout << "mark single size different." << endl;
		return 0;
	}
	
	//Compare assembly size
	if(templ.mark_type == 1)//Circle
	{
		if(img.assembly_diameter > (templ.assembly_diameter + 2*templ.assembly_margin) || img.assembly_diameter < (templ.assembly_diameter - 2*templ.assembly_margin))
		{
			cout << "mark assembly size different." << endl;
			return 0;
		}		
	}

	
	//Use filter if it turn on. In fact, it's a LUT function in OpenCV.
	if(templ.level_correct_on != 0)
	{
		Mat lookup_table(1, 256, CV_8UC1);
		uchar *p = lookup_table.data;
		for(int i=0; i<256; ++i)
		{
			if(i <=  templ.level_correct_min)
				p[i] = 0;
			else if(i >= templ.level_correct_max)
				p[i] = 255;
			else
				p[i] = (int)(pow((double)(i - templ.level_correct_min)/(templ.level_correct_max - templ.level_correct_min), templ.gamma) * 255);
		}	
		LUT(img.img, lookup_table, img.img);
		LUT(templ.templ, lookup_table, templ.templ);
	}

	//Blur the img and run template matching in low resolution to make a mask for minMaxLoc.
	Mat blurred_img(img.img.size(), CV_8UC1, Scalar(0));
	blur(img.img, blurred_img, Size(1,1));
	Mat blurred_templ(templ.templ.size(), CV_8UC1, Scalar(0));
	blur(templ.templ, blurred_templ, Size(1,1));
	
	//Make the blurred image border.
	copyMakeBorder(blurred_img, blurred_img, templ.template_height/2, templ.template_height/2, templ.template_width/2, templ.template_width/2, BORDER_REFLECT_101); 
	
	//Template matching(low res).
	Mat match_result(blurred_img.rows-blurred_templ.rows+1, blurred_img.cols-blurred_templ.cols+1, CV_64FC1, Scalar(0.0));//The result matrix has to use the "float number" element CV_32FC1
	matchTemplate(blurred_img, blurred_templ, match_result, CV_TM_CCORR_NORMED, templ.mask);
	threshold(match_result, match_result, templ.low_res, 1, THRESH_TOZERO);

	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(match_result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());//Mat() call the empty matrix feed in as mask.
	
	if(maxVal <= templ.low_res)
	{
		cout << "low-resolution template matching did not cross the threshold." << endl;
		return 0;
	}
		
	Mat minMax_mask(match_result.size(), CV_8UC1, Scalar(0));
	match_result.convertTo(minMax_mask, CV_8UC1, 255);//If use match_result.copyTo(minMax_mask), the type of minMax_mask will be change to CV_32F, which will cause error in next matchTemplate.

	//Pad the image size to (img_height + template_height -1) * (img_width + template_width -1) by fill zeros.
	copyMakeBorder(img.img, img.img, templ.template_height/2, templ.template_height/2, templ.template_width/2, templ.template_width/2, BORDER_REFLECT_101);
	img.img_height = img.img.rows;
	img.img_width = img.img.cols;	

	//Template matching with TM_CCORR_NORMED
	//Template matching(high res).
	matchTemplate(img.img, templ.templ, match_result, CV_TM_CCORR_NORMED, templ.mask);

	minMaxLoc(match_result, &minVal, &maxVal, &minLoc, &maxLoc, minMax_mask);//Mat() call the empty matrix feed in as mask.
	
	if(maxVal >= templ.high_res)
	{
		//If the mark type is single, check whether the max score is in the range of margin.
		if(templ.mark_type == 0)//Single
		{
			cout << "The template type is single." << endl;
			//if circle fit on
			Point2f circle_location(maxLoc);
			cout << "checking location:" << circle_location << endl;
			vector<vector<Point>> edges;
			if(templ.circle_fit_on == 1)
				if((circle_fit(circle_location, edges, templ, img, match_result) - 0) < DBL_EPSILON)
					return false;
			maxVal = match_result.at<float>(circle_location);//If fit edges, the circle location would be moved to the center of best-fit circle. Otherwise, circle_location unchanged
			
			cvtColor(img.img, img.img, CV_GRAY2BGR);
			//draw edges
			for(int i=0; i<edges.size(); ++i)
			{
				for(int j=0; j<edges[i].size(); ++j)
					circle(img.img, edges[i][j], 1, Scalar(0, 0, 255));
			}
			//draw center
			circle_location.x = circle_location.x + templ.center_x;
			circle_location.y = circle_location.y + templ.center_y;
			circle(img.img, circle_location, 3, Scalar(0,255,0), -1, 8, 0 );
			
			imwrite(result_path, img.img);
			return maxVal;
		}
		else//If the mark type is circle, we first find as many as 12 circles by filling searched area after found one circle. Then we check whether the circles are in the range of margin.
		{	
			cout << "The template type is circle." << endl;
			int circle_number = 0;
			vector<Point2f> circles_location;
			vector<vector<Point>> edges;
			double best_fit_diameter = 0;
			Mat best_fit_circle;
			find_multi_max(match_result, circles_location, edges, circle_number, best_fit_diameter, templ, img, best_fit_circle, minMax_mask);
			img.circles_in_margin = circle_number;
			cout << "1st matched mark number:" << img.circles_in_margin << endl;
			
			if(circle_number < 6)
				return 0;
				
			//If the circle_number < 12, find_multi_max will search circles until the number reach 12.
			find_multi_max(match_result, circles_location, edges, circle_number, best_fit_diameter, templ, img, best_fit_circle, best_fit_circle);
			img.circles_in_margin = circle_number;
			cout << "2nd matched mark number:" << img.circles_in_margin << endl;
				
			RotatedRect fit_ellipse = fitEllipse(Mat(circles_location));//fitEllipse needed to feed in CV_32FC2 datetype 
	
			if(!check_circularity(best_fit_diameter, templ.assembly_margin, fit_ellipse))
			{
				return 0;
			}
			if(img.circles_in_margin >= 6)
			{
				cvtColor(img.img, img.img, CV_GRAY2BGR);
				//draw edges and centers
				for(int i=0; i<edges.size(); ++i)
				{
					for(int j=0; j<edges[i].size(); ++j)
						circle(img.img, edges[i][j], 1, Scalar(0, 0, 255));
					circle(img.img, Point(circles_location[i].x + templ.center_x, circles_location[i].y + templ.center_y), 3, Scalar(0,255,0), -1, 8, 0);
				}
				imwrite(result_path, img.img);
				return ((double)img.circles_in_margin/(double)templ.mark_number);
			}
			return 0;
		}
	}
	else//maxVal < templ.high_res
	{
		cout << "high-resolution template matching did not cross the threshold." << endl;
		return 0;
	}
}


