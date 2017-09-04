#ifndef CLASSIFIER
#define CLASSIFIER

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>//memcpy
#include <vector>
#include<math.h>//pow

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace std;
using namespace cv;

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

img_par();
img_par(string file_path);
void initialize_parameter();
uint32_t get_offset(ifstream& in_file, uint32_t current_pos);

template <class T>
void get_content(ifstream&, uint32_t, T&, const int&, const string = "int_or_double");

void read_image(ifstream&, Mat&, int&);
};

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

#endif
