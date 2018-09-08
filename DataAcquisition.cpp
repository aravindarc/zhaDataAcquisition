#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

void sortRects(vector<Rect> &);
bool sortTeacherX(Rect, Rect);
bool sortTeacherY(Rect, Rect);

const float ALPHA = 1.6;
const uchar NOISE_THRESH = 100;

int main(int argc, char **argv) {

    if(argc == 1) {
        cerr << "File name and storage path not passed" << endl;
        return -1;
    }

    if(argc > 3) {
        cerr << "Only one argument is allowed" << endl;
        return -1;
    }

    //input image
    string filename(argv[1]);                          
    string storagePath(argv[2]);

    //pipeline it into opencv Mat object
    Mat src = imread(filename);                        

    if(!src.data) {

        //if image is corrupt or null throw error
        cerr << "Problem loading " << filename << endl;   
        return -1;
    }

    Mat rsz;
    Size size(1275, 1754);
    resize(src, rsz, size);

    //this will be used to get grayscale image
    Mat gray;       

    //color to grayscale conversion
    if(rsz.channels() == 3)
        cvtColor(rsz, gray, CV_BGR2GRAY);       
    else
        gray = rsz;

    //this will be used to store binary image
    Mat bw;        

    //grayscale to binary conversion
    adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);        

    //imwrite("bin.jpg", bw);
    Mat horizontal = bw.clone();
    Mat vertical = bw.clone();

    int scale = 30;

    int horizontalsize = horizontal.cols / scale;

    //create a Mat with only the horizontal lines
    Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize,1));        

    erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
    dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));
    //imshow("Horizontal", horizontal);

    int verticalsize = vertical.rows / scale;

    //create a Mat with only vertical lines
    Mat verticalStructure = getStructuringElement(MORPH_RECT, Size( 1,verticalsize));           

    erode(vertical, vertical, verticalStructure, Point(-1, -1));
    dilate(vertical, vertical, verticalStructure, Point(-1, -1));
    //imshow("Vertical", vertical);

    //Add the vertical and horizontal lines to get the grid mask
    Mat mask = horizontal + vertical;   

    //imshow("Mask", mask);

    vector<Vec4i> hierarchy;

    //this'll hold the contours
    std::vector<std::vector<cv::Point> > contours;      
    
    /*
     *find the contours from the mask
     *which'll be the individual character spaces
     *also other unwanted contours
     */
    cv::findContours(mask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));        

    vector<vector<Point>> contours_poly( contours.size() );

    /*
     *this'll hold the rectangle data of the contour(x, y, width, height)
     *x and y are origins of the contour, top-leftmost pixel coordinates
     */
    vector<Rect> boundRect( contours.size() );          

    //this'll hold the character rects to be filtered from other rects
    vector<Rect> selectedRects;         

    for (size_t i = 0; i < contours.size(); i++)
    {

        //fit polygonal shape over the contours
        approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );    

        //extract rect data of all possible polygonal contours
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );           

        /*
         * if the rect is square shaped with a MOE of 10 and area is
         * more than 20 pixels proceed to store, it should be characters
         */
        if(boundRect[i].width - boundRect[i].height > 10
        || boundRect[i].width - boundRect[i].height < -10
        || boundRect[i].width * boundRect[i].height < 20)               
            continue;                                                   

        /* 
         * crop and resize to 65 X 65, to remove any vestiges of
         * the borders
         */
        boundRect[i].x += 5;                                            
        boundRect[i].y += 5;                                            
        boundRect[i].width = 65;
        boundRect[i].height = 65;

        //push character rect data to a separate vector
        selectedRects.push_back(boundRect[i]);                          
    }

    //check if any discrepancy crept in
    if(selectedRects.size() != 266) {                                   
        cout << selectedRects.size() << endl;
        return -1;
    }

    //the contours will be disordered sort it using custom function
    sortRects(selectedRects);                                           
    cout << selectedRects.size() << endl;


    Mat tempBinary;

    //numerical label for character images will be generated from 0
    size_t c = 0;           
    size_t n = 0;  

    //uncomment to use         
    //Size imgWriteSize = Size(28, 28);
    
    Mat imgWrite;
    
    for(size_t i = 0; i < selectedRects.size(); i++)
    {

        ostringstream tempostr;

        //the last column alone will be saved using labels "sx" since they contain special characters
        if((i+1)%14 == 0)   
            tempostr << 's' << c++;
        else
            tempostr << n++;
        string temp = storagePath + tempostr.str() + ".png";

        tempBinary = ~gray(selectedRects[i]).clone();

        imgWrite = Mat::zeros(tempBinary.size(), tempBinary.type());
        for(int y=0; y<tempBinary.rows; y++)
            for(int x=0; x<tempBinary.cols; x++) {

                if(tempBinary.at<uchar>(y, x) > NOISE_THRESH)
                    imgWrite.at<uchar>(y, x) = saturate_cast<uchar>(ALPHA * tempBinary.at<uchar>(y, x));
                else
                    imgWrite.at<uchar>(y, x) = 0;

            }
        //uncomment to use
        //resize(tempBinary, imgWrite, imgWriteSize);

        //write the image as file
        imwrite(temp, imgWrite);     
    }
    return 0;
}

bool sortTeacherY(Rect i, Rect j) {
    return i.y < j.y;
}

bool sortTeacherX(Rect i, Rect j) {
    return i.x < j.x;
}

void sortRects(vector<Rect> &boundRect) {           
    /*
     * interesting sorting situation, since data is of the form (x, y)
     * and sorting order should be left to right and top to bottom
     * study if interested
     */
    int i;

    sort(boundRect.begin(), boundRect.end(), sortTeacherY);

    for(i=0; i<boundRect.size(); i+=14)
        sort(boundRect.begin()+i, boundRect.begin()+i+14, sortTeacherX);
}
