#include "matrixAlgorithms.h"

void MatrixCalculations::depthOfFieldCalculation(int **shift_matrix, float parameter_f, float parameter_B, int size)
{
  float **matrix = new float *[size];
  for (int i = 0 ; i < size; i++)
    matrix[i] = new float [size];


  //int parameter_y;
  for (int i = 0; i < size ; i++)
  {
    for (int j = 0 ; j < size ; j++)
    {
      //parameter_y = size - j;
      if (shift_matrix[i][j]!=0){
        matrix[i][j] = parameter_f*parameter_B/shift_matrix[i][j];
      }
      else matrix[i][j] = 0;
    
    }
  }
  MatrixCalculations::helperLoggerF(matrix, size);
}

void MatrixCalculations::marginCalculation(int &total_compatibility, int &total_compatibility_cols_taken, cv::Mat &img_total_output1, 
                                           cv::Mat &img_total_output2, cv::Mat img_first, cv::Mat img_second)
{
  //bufor variables
  int cols_taken = 0; // will hold how many columns have been taken
  int images_compatibility = 0; // compatibility counter 
  uint8_t digit; // pixel.at(i,j) of first image
  uint8_t digit2; // same of right image
  cv::Mat buf1 = img_first; // bufor Mat to find a margins on it
  cv::Mat buf2 = img_second; // same
  int check = buf1.cols/2;
  while (buf1.cols != 0 || buf1.cols > check)
  {
    buf1.release(); 
    buf2.release();
    buf1 = img_first(cv::Rect(cols_taken,0,img_first.cols-cols_taken, img_first.rows)); // cutting cols from left
    buf2 = img_second(cv::Rect(0,0,img_second.cols-cols_taken, img_second.rows)); // cutting cols from right 
    for (int i = 0 ; i < buf1.rows; i++)
    {
      for (int j = 0 ; j < buf1.cols; j++)
      {
        digit = buf1.at<uint8_t>(i,j);
        digit2 = buf2.at<uint8_t>(i,j);
        if (digit == digit2) images_compatibility++; 
      }
    }
        
    if (images_compatibility > total_compatibility)
    {    
        total_compatibility = images_compatibility;
        total_compatibility_cols_taken = cols_taken;
        img_total_output1 = buf1;
        img_total_output2 = buf2; 
    }
    cols_taken++;
    images_compatibility = 0;  
  }
}

void MatrixCalculations::helperLoggerF(float ** x, int len)
{

  std::ofstream myfile;
  myfile.open ("MacierzPrzesuniec.txt");
  for (int i = 0 ; i < len; i++){
    for (int j = 0 ; j < len; j++)
    {
        
      myfile << x[i][j]<< "   ";
    }
    myfile << std::endl;
  }
     
  myfile.close();
  return ;
} 

void MatrixCalculations::helperLogger(int ** x, int len)
{

  std::ofstream myfile;
  myfile.open ("MacierzPrzesuniec.txt");
  for (int i = 0 ; i < len; i++){
    for (int j = 0 ; j < len; j++)
    {
        
      myfile << x[i][j]<< "   ";
    }
    myfile << std::endl;
  }
     
  myfile.close();
  return ;
} 

bool MatrixCalculations::AreTherePixelsOtherThanWhite(cv::Mat m, int x, int y, int marg_len)
{
  int notWhite = 0;
  int iloop_s = x - 1 ;
  int jloop_s = y - 1 ;
  int iloop_e = x + 1;
  int jloop_e = y + 1;
  for (int i = iloop_s ; i < iloop_e; i++)
  {
    for (int j = jloop_s; j < jloop_e; j++) 
    {
      if (m.at<uint8_t>(i,j)!=255) notWhite++;
    }
  }
  if (notWhite!=0) return true;
  else return false;
}

void MatrixCalculations::calcCorrelationSkip(int **shift_matrix, cv::Mat RightSrc,cv::Mat leftC, int len, int marg_len, CProgressCtrl *prog)
{
  int shift = 0;
  int best_correlation = 0;
  int loop = len - marg_len;
  int z = (len - (marg_len*2)) / 2;
  prog->SetRange(marg_len,loop-1);
  prog->SetStep(1);
  prog->EnableWindow(1);
  for (int i = marg_len ; i < loop; ++i)
  {
    prog->StepIt();


    best_correlation = 0;
    for (int j = marg_len ; j < loop; ++j)
    {
      shift = 0;
      if (AreTherePixelsOtherThanWhite(leftC, i , j, marg_len))
      {
        while (shift<z)
        {
          squareCorrelationCheck(RightSrc, leftC, best_correlation, shift_matrix, i, j, len, marg_len, shift);
          ++shift;
        } 
      }
    }
  }
  prog->EnableWindow(0);
  prog->SetPos(0);
}


void MatrixCalculations::calcCorrelation(int **shift_matrix, cv::Mat RightSrc,cv::Mat leftC, int len, int marg_len, CProgressCtrl *prog)
{
  int shift = 0;
  int best_correlation = 0;
  int z = (len - (marg_len*2)) / 2;

  int loop = len - marg_len;
  prog->SetRange(marg_len,loop-1);
  prog->SetStep(1);
  prog->EnableWindow(1);
  for (int i = marg_len ; i < loop; ++i)
  {
    prog->StepIt();
    best_correlation = 0;
    for (int j = marg_len ; j < loop; ++j)
    {
      shift = 0;
      while (shift<z)
      {
        squareCorrelationCheck(RightSrc, leftC, best_correlation, shift_matrix, i, j, len, marg_len, shift);
        ++shift;
      }     
    }
  }
  prog->EnableWindow(0);
  prog->SetPos(0);
}
  
void MatrixCalculations::squareCorrelationCheck(cv::Mat src, cv::Mat leftC, int &best_correlation, int **shift_matrix, int x, int y, int len, int marg_len, int shift)
{
  int ix;
  int jy;
  int correlation_bufor = 0;
  int iloop_s = x - marg_len + shift;
  int jloop_s = y - marg_len;
  int iloop_e = x + marg_len + shift;
  int jloop_e = y + marg_len;

  int srcr_e = src.rows -  marg_len;
  int srcc_e = src.cols -  marg_len; 
  for (int i = iloop_s ; i < iloop_e ; ++i)
  {
    ix = i - shift;
    for (int j = jloop_s; j < jloop_e ; ++j) 
    {
      jy = j;
      if (i > srcr_e || j > srcc_e) continue;
      if (leftC.at<uint8_t>(ix,jy)==src.at<uint8_t>(i,j)) correlation_bufor++;
    }
  }
  
  if (correlation_bufor > best_correlation)
  {
    best_correlation = correlation_bufor;
    shift_matrix[x-marg_len][y-marg_len] = shift;      
  }
}

void MatrixCalculations::mirrorMargins(cv::Mat &matrix, cv::Mat matrix_inside, cv::Size box)
{
  matrix_inside.copyTo(matrix.rowRange(box.height/2 , matrix_inside.rows+box.height/2).
      colRange(box.width/2 ,matrix_inside.cols + box.width/2));
  topLeftCornerMirror(matrix , box);
  topMidMirror(matrix , box);
  topRightCornerMirror(matrix , box);
  midLeftMirror(matrix, box);
  midRightMirror(matrix, box);
  downLeftCornerMirror(matrix,box);
  downMidMirror(matrix,box);
  downRightCornerMirror(matrix,box);
    

}
void MatrixCalculations::topLeftCornerMirror(cv::Mat &matrix, cv::Size box)
{
  //top left corner, small square
  for (int i = 0 ; i < box.height/2; i++)
  {
    for (int j = 0; j < box.width/2; j++)
    {
      matrix.at<uint8_t>(i,j) = matrix.at<uint8_t>(box.height-i, box.width-j);
    }
  }
}


void MatrixCalculations::topMidMirror(cv::Mat &matrix, cv::Size box)
{
  for (int i = 0 ; i < box.height/2; i++)
  {
    for (int j = box.width/2; j < matrix.cols - box.width/2; j++)
    {
      matrix.at<uint8_t>(i,j) = matrix.at<uint8_t>(box.height-i, j);
    }
  }
}

void MatrixCalculations::topRightCornerMirror(cv::Mat &matrix, cv::Size box)
{
  for (int i = 0 ; i < box.height/2; i++)
  {
    for (int j = matrix.cols-(box.width/2); j < matrix.cols; j++)
    {
      matrix.at<uint8_t>(i,j) = matrix.at<uint8_t>( box.height-i,  matrix.cols-1-box.width/2 - (j%(matrix.cols-(box.width/2))));
    }
  }
}
void MatrixCalculations::midLeftMirror(cv::Mat &matrix, cv::Size box)
{
  for (int i = box.height/2 ; i < matrix.rows-box.height/2; i++)
  {
    for (int j = 0; j < box.width/2; j++)
    {
      matrix.at<uint8_t>(i,j) = matrix.at<uint8_t>(i, box.width - j );
    }
  }
}
void MatrixCalculations::midRightMirror(cv::Mat &matrix, cv::Size box)
{
  for (int i = box.height/2 ; i < matrix.rows - box.height/2; i++)
  {
    for (int j = matrix.cols - box.width/2 ; j < matrix.cols; j++)
    {
      matrix.at<uint8_t>(i,j) = matrix.at<uint8_t>( i, matrix.cols - 1 - box.width/2 - j%(matrix.cols - box.width/2));
    }
  }
}

void MatrixCalculations::downLeftCornerMirror(cv::Mat &matrix, cv::Size box)
{
  for (int i = matrix.rows - box.height/2 ; i < matrix.rows; i++)
  {
    for (int j = 0 ; j < box.width/2; j++)
    {
      matrix.at<uint8_t>(i,j) = matrix.at<uint8_t>(matrix.rows - 1-  box.height/2 - i%(matrix.rows - box.height/2), box.width - j );
    }
  }
}
void MatrixCalculations::downMidMirror(cv::Mat &matrix, cv::Size box)
{
  for (int i = matrix.rows - box.height/2 ; i < matrix.rows; i++)
  {
    for (int j = box.width/2 ; j < matrix.cols - box.width/2; j++)
    {
      matrix.at<uint8_t>(i,j) = matrix.at<uint8_t>(matrix.rows - 1-  box.height/2 - i%(matrix.rows - box.height/2),j);
    }
  }
}

void MatrixCalculations::downRightCornerMirror(cv::Mat &matrix, cv::Size box)
{
  for (int i = matrix.rows - box.height/2 ; i < matrix.rows; i++)
  {
    for (int j = matrix.cols - box.width/2 ; j < matrix.cols ; j++)
    {
      matrix.at<uint8_t>(i,j) = matrix.at<uint8_t>(matrix.rows - 1 - box.width/2 - i%(matrix.cols - box.width/2),matrix.cols -1- box.width/2 - j%(matrix.cols - box.width/2));
    }
  }
}