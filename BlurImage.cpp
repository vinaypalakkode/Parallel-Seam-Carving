#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <vector>
#include <algorithm>

typedef std::vector < std::pair<int, unsigned int> > pair_vector;
typedef std::vector < std::vector<unsigned int> > vector_vector;

using namespace cv;
using namespace std;

void checkArgs(int args){


  if(args!=2){

    std::cout<<" Usage: ./<exec> <input image name> ";
    exit(0);
  }

}

bool compare(const pair<int, unsigned int>&i, const pair<int, unsigned int>&j)
{
  return i.second < j.second;
}


void displayImage( string windowName, Mat& image){

  // create window to display
  namedWindow( windowName, WINDOW_AUTOSIZE );

  // display the image
  imshow( windowName, image);

}

// helper to wait till the user hits ESC key
void waitOnEsc(){

  int wait = waitKey(0);
  if(wait == 27){
    return;
  }

}


void queryDimensions(Mat& img, int* height, int* width){


  Size s = img.size();

    *height = static_cast<int>(s.height);
    *width  = static_cast<int>(s.width);

}


void computeEnergyMap(Mat& Img , pair_vector& row1, vector_vector& enMap){


  int width, height;
  queryDimensions(Img, &height, &width);

  // compute energy

   for(int i = 0; i < height; i++){

     for(int j =0; j < width; j++){

     // for single channel
      uchar curr = Img.at<uchar>(i,j);
      uchar bott = Img.at<uchar>(i+1,j);
      uchar right= Img.at<uchar>(i,j+1);

      char dx = curr - right;
      char dy = curr - bott;
      unsigned int gradSum = (dx*dx) + (dy*dy);

     // for multi-channel
      // TODO:

     // write the value back to the vector


     enMap[i][j] = gradSum;

     // populate the row1 vector of <index, energy> pair
     if(i == 0){

      pair<int, unsigned int> idxEnergy;
      idxEnergy.first  = gradSum;
      idxEnergy.second = j;

      row1.push_back( idxEnergy );

     }
    }
   }
}

unsigned char getEnergy (int j, int i, int height, int width, vector_vector& energyMap){

  if ((j < 0 || j >= height) ||
      (i < 0 || i >= width)) {

    return UCHAR_MAX;
   }

  return energyMap[j][i];
}


//get minimum energy value
unsigned char minEnergy(int row , int col, int height, int width, vector_vector& energyMap)
{
  unsigned char left = getEnergy(row-1, col-1, height, width, energyMap);
  unsigned char middle = getEnergy(row-1, col, height, width, energyMap);
  unsigned char right = getEnergy(row-1, col+1, height, width, energyMap);
  unsigned char current = getEnergy(row, col, height, width, energyMap);

  return current + std::min(middle, std::min(left, right));
}

void minCostEnergy (Mat& Img, pair_vector& row1, vector_vector& energyMap)
{
  computeEnergyMap(Img, row1, energyMap);
  int width, height;
  queryDimensions(Img, &height, &width);
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      unsigned char val = minEnergy(row, col, height, width, energyMap);
      energyMap[row][col] = val;
    }
  }
}

// generate a list of indices for every seam to be removed
void computeSeamMap(vector_vector& energyMap, pair_vector& r1, int xCount, int height ,int width,  vector_vector& seamMap ){


 // generate xCount number of seams : xCount = input Width - output Width
  for(int i = 0; i < xCount; i++){

 // pick ith min from first row of energy
    int minCol = r1[i].second;

   seamMap[i][0]  = r1[i].second;

   for(int j = 1; j < height; j++){

    unsigned char left = getEnergy(j, minCol-1, height, width, energyMap);
    unsigned char middle = getEnergy(j, minCol, height, width, energyMap);
    unsigned char right = getEnergy(j, minCol+1, height, width, energyMap);

    if (left <= right && left <= middle) {
      minCol -= 1;
    }
    else if (right <= left && right <= middle) {
      minCol += 1;
    }
    seamMap[i][j] = minCol;
    energyMap[j][minCol] = UCHAR_MAX;
   }
  }
}

// remove the seam and fill the output image container
void removeSeams(cv::Mat& outImg, cv::Mat& inImg, vector_vector& seamMap, int xCount){

  // plane 1:
  for (int i = 0; i < inImg.rows; i++){
     int count = 0;
     for(int j=0; j < inImg.cols; j++){
       int found = 0;
       for (int k=0; k < xCount; k++) {
	 if (j == seamMap[k][i]) {
	   found = 1;
	   break;
	 }
       }
       if (!found) {
	   outImg.data[i*outImg.cols+count] = inImg.data[i*inImg.cols+j];
	   count++;
       }
     }
  }
}

// void testSeams(cv::Mat& testImg, cv::Mat& inImg, vector_vector& seamMap, int xCount) {
//   for (int k=0; k < xCount; k++) {
//      for (int i = 0; i < inImg.rows; i++){
//        for(int j=0; j < inImg.cols; j++){
//  	 if (j == seamMap[k][i]) {
//  	   //  testImg.data[i*inImg.cols+j] = 255;

//  	   testImg.at<uchar>(i,j) = 255;
//             //cout << "( "<<i << ", "<<j<<" )";
//  	   if (k == 2) {
//  	     std::cout << std::endl;
//  	   }

//  	 }
//  	 else {
//  	   testImg.data[i*inImg.cols+j] = inImg.data[i*inImg.cols+j];

//  	 }
//        }
//      }
//    }
// }

// void testSeams(cv::Mat& testImg, cv::Mat& inImg, vector_vector& seamMap, int xCount) {

//   for (int k = 0 ; k < xCount; k++) {
//     for (int i = 0; i < inImg.rows; i++) {
// 	int remCol = seamMap[k][i];
// 	testImg.at<uchar>(i,remCol) = 255;
//     }
//   }
// }




int main( int argc, char* argv[]  ){


  // sanity check for commandline
  checkArgs(argc);

 // read image to Mat
  cv::Mat inImg = imread(argv[1],1);
  displayImage("input image", inImg);

  int width, height;

  int yCount = 0;

  queryDimensions(inImg, &height, &width);
  int xCount = 40; // number of seams to be removed

  // declarations
  vector_vector energyMap(height, vector<unsigned int>(width,1));

  // seam map should be a map
  vector_vector seamMap( xCount, vector <unsigned int>(height,1));
  pair_vector  row1;

  pair_vector& r1 = row1;
  vector_vector& enMap = energyMap;
  vector_vector& rSeamMap = seamMap;


 // right now use grayScale image for energy compute
  cv::Mat imGray;
  cvtColor( inImg, imGray,CV_BGR2GRAY);


  // calculate the energyMap
  minCostEnergy(imGray, r1, enMap);

  //create a data structure with <index,value> pair vector, then sort by value
  sort(r1.begin(), r1.end() );
  cout << r1[0].second<<" " << r1[1].second << "r1\n";


  // [1] Create Seams one by one
  computeSeamMap(enMap,r1, xCount, height, width, rSeamMap );

  // container for the output image
  cv::Mat outImg(height,width-xCount,CV_8UC1);
  //cv::Mat testImg(height, width, CV_8UC1);
  cv::Mat testImg = imGray.clone();

  // [2] Remove seams one by one.

  removeSeams(outImg, imGray, seamMap, xCount);
  //testSeams(testImg, imGray, seamMap, xCount);

  // write the image back and show the image
  displayImage("output image", outImg);
  //displayImage("test image", testImg);

  waitOnEsc();

 return 0;
}





