#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

void sortRects(vector<Rect> &);
bool sortTeacherX(Rect, Rect);
bool sortTeacherY(Rect, Rect);

int main() {

    string filename = "RawImages/test5.jpg";     //input image
    Mat src = imread(filename);                  //pipeline it into opencv Mat object

    if(!src.data)
        cerr << "Problem loading " << filename << endl;   //if image is corrupt or null throw error

    Mat rsz = src;

    Mat gray;       //this will be used to get grayscale image

    if(rsz.channels() == 3)
        cvtColor(rsz, gray, CV_BGR2GRAY);       //color to grayscale conversion
    else
        gray = rsz;

    Mat bw;         //this will be used to store binary image
    adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);        //grayscale to binary conversion

    Mat horizontal = bw.clone();
    Mat vertical = bw.clone();

    int scale = 30;

    int horizontalsize = horizontal.cols / scale;

    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize,1));        //create a Mat with only the horizontal lines

    erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
    dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));
    //imshow("Horizontal", horizontal);

    int verticalsize = vertical.rows / scale;
    Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));           //create a Mat with only vertical lines

    erode(vertical, vertical, verticalStructure, Point(-1, -1));
    dilate(vertical, vertical, verticalStructure, Point(-1, -1));
    //imshow("Vertical", vertical);

    Mat mask = horizontal + vertical;       //Add the vertical and horizontal lines to get the grid mask
    //imshow("Mask", mask);

    vector<Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;      //this'll hold the contours
    cv::findContours(mask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));        //find the contours from the mask
                                                                                                            //which'll be the individual character spaces
                                                                                                            //also other unwanted contours

    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );          //this'll hold the rectangle data of the contour(x, y, width, height)
                                                        //x and y are origins of the contour, top-leftmost pixel coordinates

    vector<Rect> selectedRects;         //this'll hold the character rects to be filtered from other rects

    for (size_t i = 0; i < contours.size(); i++)
    {
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );    //fit polygonal shape over the contours
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );           //extract rect data of all possible polygonal contours


        if(boundRect[i].width - boundRect[i].height > 10
        || boundRect[i].width - boundRect[i].height < -10
        || boundRect[i].width * boundRect[i].height < 20)               //if the rect is square shaped with a MOE of 10 and area is
            continue;                                                   //more than 20 pixels proceed to store, it should be characters

        selectedRects.push_back(boundRect[i]);                          //push character rect data to a separate vector
    }

    if(selectedRects.size() != 247) {                                   //check if any discrepancy crept in
        cout << "Contours are more or less than 247" << endl;
        return -1;
    }

    sortRects(selectedRects);                                           //the contours will be disordered sort it using custom function
    cout << "Number of images saved: " << selectedRects.size() << endl;

    Mat tempBinary;
    gray = bw-mask;         //try and remove the grid from the original binary image
                            //note that some pixels of the grid will persist

    for(size_t i = 0; i < selectedRects.size(); i++)
    {
        ostringstream tempostr;
        tempostr << i;
        string temp = "ResultantStorage/" + tempostr.str() + ".jpg";

        adaptiveThreshold(gray(selectedRects[i]).clone(), tempBinary, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);
                                                                    //crop the image with the rect data and convert it to binary
                                                                    //unsure this line is needed, experiment

        tempBinary = ~tempBinary;      //Comment this line to toggle binary color
        imwrite(temp, tempBinary);     //write the image as file
    }
    return 0;
}

bool sortTeacherY(Rect i, Rect j) {
    return i.y < j.y;
}

bool sortTeacherX(Rect i, Rect j) {
    return i.x < j.x;
}

void sortRects(vector<Rect> &boundRect) {           //interesting sorting situation, since two points are the data
                                                    //and sorting order should be left to right and top to bottom
    int i;                                          //study if interested

    sort(boundRect.begin(), boundRect.end(), sortTeacherY);

    for(i=0; i<boundRect.size(); i+=13)
        sort(boundRect.begin()+i, boundRect.begin()+i+13, sortTeacherX);
}
