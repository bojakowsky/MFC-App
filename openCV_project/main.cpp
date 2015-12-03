//-----------------------------------------------------------------------------------------
#include <afxwin.h>      //MFC core and standard component
#include <atlimage.h>    //included for CImage
#include <afxdlgs.h>
#include <afxcmn.h>
#include "resource1.h"   //main symbols
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdint.h>
#include <fstream>
#include "matrixAlgorithms.h"
//-----------------------------------------------------------------------------------------

class PROJECT_FORM : public CDialog
{
   public:
    PROJECT_FORM(CWnd* pParent = NULL): CDialog(PROJECT_FORM::IDD, pParent)
    {    }

    // Dialog Data, name of dialog form
    enum{IDD = IDD_INTERFACE};

    protected:
    virtual void DoDataExchange(CDataExchange* pDX) { CDialog::DoDataExchange(pDX); }
    //For database usage
    //Called right after constructor. Initialize things here.
    virtual BOOL OnInitDialog() 
    { 
            CDialog::OnInitDialog();
            initializeWindowElements();
            return true; 
    }
public:
  void initializeWindowElements();
  void marginCalculating(); // calculate margins
  char *CStringToCharPtr(int which); // returns path of the image, that user've choosen - converting CString into *char
  int calcZeros(uint8_t val); // calculating number of 0 in pixel(max 255)
  void full_screen_second_image_onClick(); // enables the image to be shown up in full size
  void full_screen_first_image_onClick(); // 
  void correlation_and_fitting(); // makes correlation and fitting of images, the shift matrix values are held in D:/
  void do_step(); // steps of the program 
  void image_loading_without_popup(CString path, int which); // loads image into ImageControl without poping up browser
  void load_first_image(); // first image browser
  void load_second_image(); // second image browser
  void image_loading(int which); // loading image into ImageControl - poping up browser
  void overwriteImage(cv::Mat img_first, cv::Mat img_second);
  void initialProcessing();
  void thresholdingImage();
  void depthOfField();
  CEdit   *pPROGRESS; // progress text field
  CButton *pLOAD_FIRST_IMAGE; // load button (1)
  CButton *pLOAD_SECOND_IMAGE; // load button (2)
  CButton *pNEXT_STEP; // step button 

  CStatic *pFIRST_IMAGE; // first image (left)
  CStatic *pSECOND_IMAGE; // second image (right)

  bool first_image_loaded; // false; // checks if first image has been loaded
  bool second_image_loaded; // false // -||- if second

  CStringA first_path_bufor; // = ""; // path to the first image 
  CStringA second_path_bufor; // = ""; // path to the second image
  CStringA initial_first_path;
  CStringA initial_second_path;
  short stage; // = 0; // in what stage of app we are

  cv::Mat left_image_without_margins; // after loading image by user and cutting the marings it will be hold here
  cv::Mat right_image_without_margins; // same as left
  int margin; // number of columns we cut from image (from 1 side)
  cv::Mat left_image_margin; // used for displaying
  cv::Mat right_image_margin; // used for displaying

  CComboBox *pSIZE_OF_THE_BOX;
  int size_of_the_box;

  CEdit *pPARAMETER_F;
  float parameter_f;

  int shift_matrix_size;

  CEdit *pPARAMETER_B;
  float parameter_B;
  
  CButton *pSHOULDTHRESHOLDIMAGE;
  CButton *pSHOULDPASSAFTERTHRESHOLD;

  CProgressCtrl *pPROGBAR;

  bool shouldThresholdImage;
  bool shouldPassAfterThreshold;

  cv::Mat img_first;
  cv::Mat img_second;

  int **shift_matrix;
  void onPaint()
  {
    CPaintDC dc(this);
  } 

DECLARE_MESSAGE_MAP()
//------------------------------------------------------------------------
  afx_msg void Mdo_step(){ do_step(); }
  afx_msg void Mload_first_image(){ load_first_image(); }
  afx_msg void Mload_second_image(){ load_second_image(); }
  afx_msg void Mfull_screen_first_image(){ full_screen_first_image_onClick(); }
  afx_msg void Mfull_screen_second_image(){ full_screen_second_image_onClick(); }
};


class TheProject : public CWinApp
{
public:
  TheProject() {  }
  virtual BOOL InitInstance()
  {
    CWinApp::InitInstance();
    PROJECT_FORM dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    return FALSE;
  } //close function

};

//-----------------------------------------------------------------------------------------
//Need a Message Map Macro for both CDialog and CWinApp

BEGIN_MESSAGE_MAP(PROJECT_FORM, CDialog)
    ON_COMMAND(NEXTSTEP, Mdo_step)
    ON_COMMAND(LOADIMAGE1, Mload_first_image)
    ON_COMMAND(LOADIMAGE2, Mload_second_image)
    ON_STN_CLICKED(IMAGE1, Mfull_screen_first_image)
    ON_STN_CLICKED(IMAGE2, Mfull_screen_second_image)
    ON_WM_PAINT()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------------------

TheProject theApp;  //Starts the Application


void PROJECT_FORM :: initializeWindowElements()
{

    //Initialization
    first_image_loaded = false;
    second_image_loaded = false;
    first_path_bufor = ""; 
    second_path_bufor = "";
    stage = 0;

    //Initialization of window all window elements
    pPROGRESS = (CEdit *) GetDlgItem(PROGRESS); 
    pLOAD_FIRST_IMAGE = (CButton *) GetDlgItem(LOADIMAGE1);
    pLOAD_SECOND_IMAGE = (CButton *) GetDlgItem(LOADIMAGE2);
    pNEXT_STEP = (CButton *) GetDlgItem(NEXTSTEP);
    pNEXT_STEP->EnableWindow(FALSE);
    pFIRST_IMAGE = (CStatic *) GetDlgItem(IMAGE1);
    pSECOND_IMAGE = (CStatic *) GetDlgItem(IMAGE2);
    pSIZE_OF_THE_BOX = (CComboBox *) GetDlgItem(DROPDOWN);
    pSIZE_OF_THE_BOX->EnableWindow(0);
    /*pSIZE_OF_THE_BOX->AddString(_T("2"));
    pSIZE_OF_THE_BOX->AddString(_T("4"));
    pSIZE_OF_THE_BOX->AddString(_T("8"));
    pSIZE_OF_THE_BOX->AddString(_T("16"));
    pSIZE_OF_THE_BOX->AddString(_T("32"));
    pSIZE_OF_THE_BOX->AddString(_T("64"));
    pSIZE_OF_THE_BOX->AddString(_T("128"));
    pSIZE_OF_THE_BOX->AddString(_T("256"));
    pSIZE_OF_THE_BOX->AddString(_T("512"));*/
    pSIZE_OF_THE_BOX->AddString(_T("3"));
    pSIZE_OF_THE_BOX->AddString(_T("5"));
    pSIZE_OF_THE_BOX->AddString(_T("7"));
    pSIZE_OF_THE_BOX->AddString(_T("9"));
    pSIZE_OF_THE_BOX->AddString(_T("11"));
    pSIZE_OF_THE_BOX->AddString(_T("13"));
    pSIZE_OF_THE_BOX->AddString(_T("15"));
    pSIZE_OF_THE_BOX->AddString(_T("17"));
    pSIZE_OF_THE_BOX->AddString(_T("19"));
    pSIZE_OF_THE_BOX->AddString(_T("21"));
    pSIZE_OF_THE_BOX->AddString(_T("23"));
    pSIZE_OF_THE_BOX->AddString(_T("25"));
    pSIZE_OF_THE_BOX->AddString(_T("27"));
    pSIZE_OF_THE_BOX->AddString(_T("29"));
    pSIZE_OF_THE_BOX->AddString(_T("31"));
    pSIZE_OF_THE_BOX->AddString(_T("33"));
    pSIZE_OF_THE_BOX->AddString(_T("35"));
    pPARAMETER_F = (CEdit *) GetDlgItem(PARF);
    pPARAMETER_F->EnableWindow(0);
    pPARAMETER_B = (CEdit *) GetDlgItem(PARB);
    pPARAMETER_B->EnableWindow(0);
    pSHOULDTHRESHOLDIMAGE = (CButton *) GetDlgItem(CHECK1);
    pSHOULDTHRESHOLDIMAGE->EnableWindow(0);
    pSHOULDPASSAFTERTHRESHOLD = (CButton *) GetDlgItem(CHECK2);
    pSHOULDPASSAFTERTHRESHOLD->EnableWindow(0);
    pPROGBAR = (CProgressCtrl *) GetDlgItem(PROGBAR);
}

void PROJECT_FORM :: marginCalculating()
{
    
    //result variables
    int total_compatibility = 1; // best compatibility found 
    int total_compatibility_cols_taken = 1; // number of columns taken when best compatibility found
    cv::Mat img_total_output1 = img_first; // this variable will hold image without margins
    cv::Mat img_total_output2 = img_second; // same

    MatrixCalculations::marginCalculation(total_compatibility, total_compatibility_cols_taken, img_total_output1, img_total_output2, img_first, img_second);

    // creating output to display it on screen as result of margin deleting
    cv::Mat total_output (img_first.rows, img_first.cols + total_compatibility_cols_taken, CV_8U); 
      
    ////Saving for further use 
    margin = total_compatibility_cols_taken; // saving margin value 
    left_image_without_margins = img_total_output1.clone(); // saving left image 
    right_image_without_margins = img_total_output2.clone(); // saving right image

    cv::Mat left = img_first(cv::Rect(0 , 0 , total_compatibility_cols_taken, img_first.rows)); // creating Mat to hold left margin
    cv::Mat right = img_second(cv::Rect(img_second.cols-total_compatibility_cols_taken, 0 ,total_compatibility_cols_taken,img_second.rows));

    ////Saving for further use
    right_image_margin = right; // saving right margin image
    left_image_margin = left; // saving left margin image 
    ///////

    // adding left margin image to image which will be displayed
    if (total_compatibility_cols_taken!=0)
      left.copyTo(total_output.rowRange(0,img_total_output1.rows).colRange(0,total_compatibility_cols_taken));

    // it's averaging the pixels of the left and right image (without margins) to show differences between them
    for (int i = 0; i < img_total_output1.rows; i++){
      for (int j = 0 ; j < img_total_output1.cols; j++){
        img_total_output1.at<uint8_t>(i,j) = (img_total_output1.at<uint8_t>(i,j) + img_total_output2.at<uint8_t>(i,j))/2;
      }
    }
    img_total_output1.copyTo(total_output.rowRange(0,img_total_output1.rows).
      colRange(total_compatibility_cols_taken,img_total_output1.cols+total_compatibility_cols_taken));

    // adding right margin image to image which will be displayed
    if (total_compatibility_cols_taken!=0){
        right.copyTo(total_output.rowRange(0,img_total_output1.rows).
          colRange(img_total_output1.cols+total_compatibility_cols_taken,img_total_output1.cols+total_compatibility_cols_taken*2));

    //drawing lines where margins are
    cv::line(total_output,cv::Point(total_compatibility_cols_taken,0),
      cv::Point(total_compatibility_cols_taken,total_output.rows),cv::Scalar(0,0,0),2);
    cv::line(total_output,cv::Point(total_output.cols-total_compatibility_cols_taken,0),
      cv::Point(total_output.cols-total_compatibility_cols_taken,total_output.rows),cv::Scalar(0,0,0),2);
    }

    cv::imshow("Wynik dzia³añ - marginesy", total_output);
}

char* PROJECT_FORM :: CStringToCharPtr(int which_path)
{
  if (which_path == 1)
  {
    const size_t new_size = (first_path_bufor.GetLength() + 1); //size of string
    char *new_string = new char[new_size]; // initiation
    strcpy_s(new_string, new_size, first_path_bufor); // copying CString into *char
    return new_string;
  }
  else 
  {
    //same but for another path
    const size_t new_size = (second_path_bufor.GetLength() + 1); 
    char *new_string = new char[new_size];
    strcpy_s(new_string, new_size, second_path_bufor);
    return new_string;
  }
    
}

void PROJECT_FORM :: full_screen_first_image_onClick()
{
  if (first_path_bufor != "")
  {
    char *path = CStringToCharPtr(1);
    std::string buf = "Pierwszy obraz: ";
    buf += path;

	  //Load image from file
	  cv::Mat src = cv::imread( path, 1 );

	  //Display image
	  imshow( buf, src );
  }
}

void PROJECT_FORM :: full_screen_second_image_onClick()
{
  if (second_path_bufor != "")
  {
    char *path = CStringToCharPtr(2);
    std::string buf = "Drugi obraz: ";
    buf += path;
	  
    //Load image from file
	  cv::Mat src = cv::imread( path, 1 );
	  //Display image
	  imshow( buf, src );
  }
}

void PROJECT_FORM :: correlation_and_fitting()
{

  //this variable scales original image and shift matrix
  shift_matrix_size = 128;

  //Resizing 
  cv::resize(left_image_without_margins,left_image_without_margins,cv::Size(shift_matrix_size,shift_matrix_size));
  cv::resize(right_image_without_margins,right_image_without_margins,cv::Size(shift_matrix_size,shift_matrix_size));

  int nIndex = pSIZE_OF_THE_BOX->GetCurSel();
  CString strCBText;
  pSIZE_OF_THE_BOX->GetLBText( nIndex, strCBText);
  
  const size_t StringSize = 4;
  size_t CharactersConverted = 0;

  char strChar[StringSize];

  wcstombs_s(&CharactersConverted, strChar, 
         strCBText.GetLength()+1, strCBText, 
         _TRUNCATE);

  size_of_the_box = atoi(strChar);

  cv::Size box(size_of_the_box*2 ,size_of_the_box*2); //holding our one box size
  // creating Mat to hold image with mirrored parts
  cv::Mat leftC(left_image_without_margins.rows + box.height, left_image_without_margins.cols + box.width , CV_8U); 
  cv::Mat rightC(left_image_without_margins.rows + box.height, left_image_without_margins.cols + box.width , CV_8U);
  MatrixCalculations::mirrorMargins(leftC, left_image_without_margins,box);
  MatrixCalculations::mirrorMargins(rightC, right_image_without_margins,box);
  cv::imshow("Lewy obraz z odbiciem marginesów",leftC);
  cv::waitKey(50);
  cv::imshow("Prawy obraz z odbiciem marginesów",rightC);
  cv::waitKey(50);
  //std::vector<cv::Mat> boxes_left; // the left image cut into boxes
  //std::vector<cv::Point2d> points_left; // the left image (x,y) pos of each box

  //Resizing it again into 512x512 
  //cv::resize(leftC,leftC,cv::Size(512,512));
  //cv::resize(rightC,rightC,cv::Size(512,512));
  
  //I do not delete these lines, cause meaby it will be used in future
  //cv::Mat buf = rightC;
  //rightC = leftC;
  //leftC = buf;

  //Cutting everything into boxes
  /// if checkbox  "wyznaczanie cech" is checked then :
  
  /*for (int y = 0; y < leftC.rows; y += box.height)
  {
    for (int x = 0; x < leftC.cols; x += box.width)
      {
          cv::Rect rect =  cv::Rect(x,y, box.width, box.height);
          boxes_left.push_back(cv::Mat(leftC, rect));
          points_left.push_back(cv::Point2d(x,y));
      }
  }*/
 
  //creating array to calculate shift matrix 
  int mat_size = leftC.rows;
  
  shift_matrix = new int *[shift_matrix_size];
  for (int i = 0 ; i < shift_matrix_size; i++)
    shift_matrix[i] = new int [shift_matrix_size];

  //initialing it with 0
  for (int i = 0 ; i < shift_matrix_size; i++)
    for (int j = 0; j< shift_matrix_size; j++)
      shift_matrix[i][j] = 0;

  uint16_t state = pSHOULDPASSAFTERTHRESHOLD->GetState();
  pNEXT_STEP->EnableWindow(0);
  if ((state & BST_CHECKED) != 0)
  {
    MatrixCalculations::calcCorrelationSkip(shift_matrix, rightC, leftC, mat_size, box.width/2, pPROGBAR);
  }
  else 
  {
    MatrixCalculations::calcCorrelation(shift_matrix, rightC, leftC, mat_size, box.width/2, pPROGBAR);
  }
  pNEXT_STEP->EnableWindow(1);
  MatrixCalculations::helperLogger(shift_matrix, shift_matrix_size);    
}
void PROJECT_FORM :: overwriteImage(cv::Mat left, cv::Mat right)
{
  cv::imwrite("left.jpg",left);
  cv::imwrite("right.jpg",right);
  image_loading_without_popup("left.jpg",1);
  image_loading_without_popup("right.jpg",2);
  first_path_bufor = "left.jpg";
  second_path_bufor = "right.jpg";
}
void PROJECT_FORM :: initialProcessing()
{
    char *first = CStringToCharPtr(1); // bufor for image path
    char *second = CStringToCharPtr(2); // same
    img_first = cv::imread(first , CV_8U); // image loaded into variable
    img_second = cv::imread(second , CV_8U); // 
    cv::resize(img_first,img_first,cv::Size(512,512));
    cv::resize(img_second,img_second,cv::Size(512,512));
    overwriteImage(img_first,img_second);
}

void PROJECT_FORM :: thresholdingImage()
{
    uint16_t state = pSHOULDTHRESHOLDIMAGE->GetState();
    if ((state & BST_CHECKED) != 0)
    {
      //Grayscale matrix
      cv::Mat grayscaleMat_f (img_first.size(), CV_8U);
      cv::Mat grayscaleMat_s (img_second.size(), CV_8U);
      //Convert BGR to Gray
      char *first = CStringToCharPtr(1); // bufor for image path
      char *second = CStringToCharPtr(2); // same
      cv::Mat left_color = cv::imread(first , CV_LOAD_IMAGE_COLOR); // image loaded into variable
      cv::Mat right_color = cv::imread(second , CV_LOAD_IMAGE_COLOR); // 
      cv::resize(left_color,left_color,cv::Size(512,512));
      cv::resize(right_color,right_color,cv::Size(512,512));

      cv::cvtColor( left_color, grayscaleMat_f, CV_BGR2GRAY );
      cv::cvtColor( right_color, grayscaleMat_s, CV_BGR2GRAY );
      //Binary image
      cv::Mat binaryMat_f(grayscaleMat_f.size(), grayscaleMat_f.type());
      cv::Mat binaryMat_s(grayscaleMat_s.size(), grayscaleMat_s.type());

      cv::threshold(grayscaleMat_f, binaryMat_f,100,255,cv::THRESH_BINARY); 
      cv::threshold(grayscaleMat_s, binaryMat_s,100,255,cv::THRESH_BINARY);

      overwriteImage(binaryMat_f,binaryMat_s);
      img_first = binaryMat_f;
      img_second = binaryMat_s;
    }
}

void PROJECT_FORM :: depthOfField()
{
  CString parf_txt; 
  pPARAMETER_F->GetWindowTextW(parf_txt);
  CString parb_txt;
  pPARAMETER_B->GetWindowTextW(parb_txt);

  const size_t StringSize = 10;
  size_t CharactersConverted = 0;

  char strChar[StringSize];

  wcstombs_s(&CharactersConverted, strChar, 
         parf_txt.GetLength()+1, parf_txt, 
         _TRUNCATE);

  parameter_f = atof(strChar);

  wcstombs_s(&CharactersConverted, strChar, 
         parb_txt.GetLength()+1, parb_txt, 
         _TRUNCATE);
  
  parameter_B = atof(strChar);

      //int parameter_y = size_of_the_box-j; // e.g. from 32 to 1
  MatrixCalculations::depthOfFieldCalculation(shift_matrix, parameter_f, parameter_B, shift_matrix_size);
  pNEXT_STEP->EnableWindow(0);

}
void PROJECT_FORM :: do_step()
{
  if (stage == 0)
  {
    stage++;
    pPROGRESS->SetWindowTextW( L"ETAP: [Przetwarzanie wstêpne] ");
    initialProcessing();
    pNEXT_STEP->EnableWindow(TRUE);
    pLOAD_FIRST_IMAGE->EnableWindow(FALSE);
    pLOAD_SECOND_IMAGE->EnableWindow(FALSE);
    pPROGRESS->SetWindowTextW( L"ETAP: [Przetwarzanie wstêpne] - zakoñczono ");
  }
  else if (stage == 1)
  {
    stage++;
    pPROGRESS->SetWindowTextW( L"ETAP: [Wydzielanie cech] - zaznacz opcje");
    pSHOULDTHRESHOLDIMAGE->EnableWindow(1);
    pSHOULDPASSAFTERTHRESHOLD->EnableWindow(1);
    //binaryzacja
  }
  else if (stage == 2)
  {
    stage ++;
    thresholdingImage();
    pPROGRESS->SetWindowTextW( L"ETAP: [Wydzielanie cech] - zakoñczono");
    pSHOULDTHRESHOLDIMAGE->EnableWindow(0);
    pSHOULDPASSAFTERTHRESHOLD->EnableWindow(0);

  }
  else if (stage == 3)
  {
    stage++;
    pPROGRESS->SetWindowTextW( L"ETAP: [Pasowanie] - trwa przetwarzanie - proszê czekaæ");
    pNEXT_STEP->EnableWindow(0);
    marginCalculating();
    pPROGRESS->SetWindowTextW( L"ETAP: [Pasowanie] - zakoñczono ");
    pNEXT_STEP->EnableWindow(1);
  } 
  else if (stage == 4)
  {
    stage++;
    pSIZE_OF_THE_BOX->EnableWindow(1);
    pPROGRESS->SetWindowTextW( L"ETAP: [Korelacja] - wybierz szerokoœæ marginesów, a nastêpnie przejdŸ dalej");
  }
  else if (stage == 5)
  {
    stage++;
     pSIZE_OF_THE_BOX->EnableWindow(0);
    pPROGRESS->SetWindowTextW( L"ETAP: [Korelacja] - trwa przetwarzanie proszê czekaæ");
    correlation_and_fitting();
    pPROGRESS->SetWindowTextW( L"ETAP: [Korelacja] - zakoñczono");
  }
  else if (stage == 6)
  {
    stage++;
    pPARAMETER_F->EnableWindow(1);
    pPARAMETER_B->EnableWindow(1);
    pPROGRESS->SetWindowTextW( L"ETAP: [Mapa g³êbi] - wpisz parametry f i B");
  }
  else if (stage == 7)
  {
    stage++;
    pPARAMETER_F->EnableWindow(0);
    pPARAMETER_B->EnableWindow(0);
    pPROGRESS->SetWindowTextW( L"ETAP: [Mapa g³êbi] - trwa obliczanie mapy g³êbi");
    depthOfField();
    pPROGRESS->SetWindowTextW( L"ETAP: [Mapa g³êbi] - zakoñczono");
  }
  
}

void PROJECT_FORM :: image_loading_without_popup(CString path, int which_image)
{
  CImage img;
  if (which_image == 1)
    img.Load((CString)path);
  else 
    img.Load((CString)path);

    int iNewWidth = 450;
	  int iNewHeight = 480;
	
	  CDC *screenDC = GetDC();
	  CDC *pMDC = new CDC;
	  pMDC->CreateCompatibleDC(screenDC);

	  CBitmap *pb = new CBitmap;
	  pb->CreateCompatibleBitmap(screenDC, iNewWidth, iNewHeight);

	  CBitmap *pob = pMDC->SelectObject(pb);
	  img.StretchBlt(pMDC->m_hDC,0, 0, iNewWidth, iNewHeight, 0, 0, img.GetWidth(), img.GetHeight(), SRCCOPY);
	  pMDC->SelectObject(pob);
    if (which_image==1)
    {
      pFIRST_IMAGE->SetBitmap((HBITMAP)(*pb));
      first_image_loaded = true;
    }
    else
    {
      pSECOND_IMAGE->SetBitmap((HBITMAP)(*pb));
      second_image_loaded = true;
    }
	  ReleaseDC(screenDC);
}

void PROJECT_FORM :: image_loading(int which_image)
{
    
  CString pathName = ""; 
  CFileDialog d(TRUE);
  if( IDOK == d.DoModal( ) )
  {
    pathName = d.GetPathName();
    UpdateData( FALSE );
  }

  CImage img;    
  img.Destroy();

  if (pathName != "" )
  {
    if (which_image==1)
    {
      first_path_bufor = pathName;
      initial_first_path = pathName;
    }
    else {
      second_path_bufor = pathName;
      initial_second_path;
    }
    
    img.Load(pathName);
    int iNewWidth = 450;
	  int iNewHeight = 450;
	
	  CDC *screenDC = GetDC();
	  CDC *pMDC = new CDC;
	  pMDC->CreateCompatibleDC(screenDC);

	  CBitmap *pb = new CBitmap;
	  pb->CreateCompatibleBitmap(screenDC, iNewWidth, iNewHeight);

	  CBitmap *pob = pMDC->SelectObject(pb);
	  img.StretchBlt(pMDC->m_hDC,0, 0, iNewWidth, iNewHeight, 0, 0, img.GetWidth(), img.GetHeight(), SRCCOPY);
	  pMDC->SelectObject(pob);
    if (which_image==1)
    {
      pFIRST_IMAGE->SetBitmap((HBITMAP)(*pb));
      first_image_loaded = true;
    }
    else
    {
      pSECOND_IMAGE->SetBitmap((HBITMAP)(*pb));
      second_image_loaded = true;
    }
	  ReleaseDC(screenDC);
  }
}

void PROJECT_FORM :: load_first_image()
{
  image_loading(1); // Loads first image - int 1 means first 
  if (first_image_loaded && second_image_loaded)
    pNEXT_STEP->EnableWindow(TRUE);
}

void PROJECT_FORM :: load_second_image()
{
  image_loading(2);
  if (first_image_loaded && second_image_loaded)
    pNEXT_STEP->EnableWindow(TRUE); 
}




