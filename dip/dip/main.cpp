//
//  main.cpp
//  dip
//
//  Created by Илья on 11/05/2019.
//  Copyright © 2019 Илья. All rights reserved.
//

/*
 vector < vector<Point2f> > point or vector < vector<Point2f> > newPositionPoint;
 первый vector это количество лиц
 второй vector это количество точек
*/


#import  <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <vector>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <cmath>

using namespace cv;
using namespace cv::face;
using namespace std;

// глобальные переменные для pointAveraging
bool start = false;
vector < vector<Point2f> > newPositionPoint;
int countFrame = 0,
    countPointLandmarsk = 68;

float oldheightBrowLeft,
      oldheightBrowRight,
      oldSBig = 0,
      oldSLittle = 0;

struct triangule
{
    Point2f position;
};

vector < vector<Point2f> > triangulation ( vector < vector<Point2f> > point )// триангуляция
{
    triangule tr;
    
    Mat img = imread("/UserPath/image.jpg");
    vector<Point2f> p(68);
    vector<Point> pt(3);
    vector<Vec6f> triangleList;
    vector<Point2f> points;
    
    for(int i = 0; i < countPointLandmarsk; i++)
    {
        p[i].x = point[0][i].x;
        p[i].y = point[0][i].y;
    }
    
    points = p;
    
    Size size = img.size();
    Rect rect(0, 0, size.width, size.height); // размер изображения
    Subdiv2D subdiv(rect);
    subdiv.insert (points);
    subdiv.getTriangleList(triangleList);
    
    for(size_t i = 0; i < triangleList.size(); ++i)
    {
        Vec6f t = triangleList[i];
        
        pt[0] = Point(cvRound(t[0]), cvRound(t[1])); // cvRound - округление
        pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
        pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
        
        line(img, pt[0], pt[1], Scalar(0,0,255), 1);
        line(img, pt[1], pt[2], Scalar(0,0,255), 1);
        line(img, pt[2], pt[0], Scalar(0,0,255), 1);
        
        float sx = (pt[0].x + pt[0].x + pt[0].x) / 3;
        float sy  = (pt[0].y + pt[0].y + pt[0].y) / 3; // подсчет площади
        
        Point f;
        f.x = sx ;
        f.y = sy ;
        
        imshow("", img);
    }
    return point;
}

vector < vector<Point2f> > positionObject ( vector < vector<Point2f> > point ) // позиция лица по отношению к камере
{
    return point;
}

vector < vector<Point2f> > calmFace ( vector < vector<Point2f> > point ) // определение спокойного лица
{
    //  определение радужки внутри точек вокруг глаз
    vector < vector<Point2f> > eyePointRight (1);
    vector < vector<Point2f> > eyePointLeft (1);
    
    for(int i = 0; i < 7; i++)
    {
        eyePointLeft[0].push_back(Point2f(0,0));
        eyePointRight[0].push_back(Point2f(0,0));
    }
    
    int leftNum = 36;
    for(int i = 0; i < 6; i++) //точки, принадлежащие левому глазу
    {
        eyePointLeft[0][i].x = point[0][leftNum].x;
        eyePointLeft[0][i].y = point[0][leftNum].y;
        ++leftNum;
    }
    
    int RightNum = 42;
    for(int i = 0; i < 6; i++)  //точки, принадлежащие правому глазу
    {
        eyePointRight[0][i].x = point[0][RightNum].x;
        eyePointRight[0][i].y = point[0][RightNum].y;
        ++RightNum;
    }
    
    for(int i = 0; i < eyePointLeft.size(); i++)
    {
        Mat Win = imread("/UserPath/image.jpg"); // нужно будет находить окружность тут
        drawFacemarks(Win, eyePointLeft[i], Scalar(0,0,255));
        drawFacemarks(Win, eyePointRight[i], Scalar(0,0,255));
        
        imshow("", Win);
    }
    
    return point;
}

float getsqr( Point2f p1, Point2f p2, Point2f p3 )
{
    Point2f
    delta,
    b;
    
    float
    height,
    lenght;
    
    b.x = abs(p2.x - p1.x) / 2; // точка b
    b.y = abs(p2.y - p1.y) / 2;
    
    
    delta.x = abs( p2.x - p1.x );
    delta.y = abs( p2.y - p1.y );
    
    delta.x = delta.x * delta.x;
    delta.y = delta.y * delta.y;
    
    lenght = sqrt(delta.x + delta.y) / 2; //  (половина b - половина длины основания треугольника)
    
    delta.x = abs(p3.x - b.x);
    delta.y = abs(p3.y - b.y);
    
    height = sqrt(delta.x - delta.y);
    return height * lenght;
}

int area () // определение эмоций
{
    Mat img = imread("/UserPath/image.jpg");
    for(int i = 0; i < countPointLandmarsk; i++)
    {
        drawFacemarks(img, newPositionPoint[i], Scalar(0,0,255));
        
        vector<Point2f> pos(4);
        
        pos[0].x = newPositionPoint[0][48].x;
        pos[0].y = newPositionPoint[0][48].y; // левый угол губы
        
        pos[1].x = newPositionPoint[0][54].x;
        pos[1].y = newPositionPoint[0][54].y; // правый угол губы
        
        pos[2].x = newPositionPoint[0][30].x;
        pos[2].y = newPositionPoint[0][30].y; // центральная точка носа
        
        pos[3].x = newPositionPoint[0][62].x;
        pos[3].y = newPositionPoint[0][62].y; // нижняя точка губы
        
        
        Point2f
        p1 = newPositionPoint[0][0],  // крайняя левая точка
        p2 = newPositionPoint[0][16], // крайняя правая точка
        p3 = newPositionPoint[0][8],  // крайняя нижняя точка
        b,
        delta;
        
        float len,
        height,
        S;
        
        b.x = (p2.x - p1.x) / 2; // точка b
        b.y = (p2.y - p1.y) / 2;
        
        delta.x =  p2.x - p1.x;
        delta.y = p2.y - p1.y;
        
        delta.x = delta.x * delta.x;
        delta.y = delta.y * delta.y;
        len =  sqrt (delta.x + delta.y) / 2; //  (половина b - половина длины основания треугольника)
        
        delta.x =  p3.x - b.x;
        delta.y =  p3.y - b.y;
        
        height = sqrt(delta.x - delta.y);
        S = height * len; // площадь общего лицевого треугольника ( точек 0 16 8 )
        
        Point2f
        eyePointLeft1 = newPositionPoint[0][41],// подсчет середины глаза
        eyePointLeft2 = newPositionPoint[0][38],
        
        eyePointRight1 = newPositionPoint[0][47],
        eyePointRight2 = newPositionPoint[0][44],
        
        eyebrowLeft = newPositionPoint[0][19],
        eyebrowRight = newPositionPoint[0][24],
        
        nbridgeleft = newPositionPoint[0][21],
        nbridgeright = newPositionPoint[0][22],
        nbridgebottom = newPositionPoint[0][27];
        
        float
        eLeft,
        eRight,
        heightBrowLeft,
        heightBrowRight,
        currentSLittle,
        currentSBig;
        
        eRight = (eyePointRight1.y - eyePointRight2.y) + eyePointRight1.y; // подсчет середины глаза в абсолютных координатах
        eLeft  = (eyePointLeft1.y  - eyePointLeft2.y)  + eyePointLeft1.y;
        
        heightBrowLeft  =  eLeft - eyebrowLeft.y;
        heightBrowRight = eRight - eyebrowRight.y; // высота подъема брови
        
        currentSLittle = getsqr ( newPositionPoint[0][27],newPositionPoint[0][21],newPositionPoint[0][22] ); // little triangle
        currentSBig = getsqr ( newPositionPoint[0][0],newPositionPoint[0][16],newPositionPoint[0][8] ); // big triangle
        
        
        //  ОПРЕДЕЛЕНИЯ ЭМОЦИИ УДИВЛЕНИЯ/ЗЛОБЫ
        
      /*   if ((oldSLittle != 0) and (oldSBig != 0))
        {
            
         if (((oldSBig * 1.1) > currentSBig) and ((oldSBig * 0.9) <= currentSBig)) // если большой не изменяется в размере
           {
             
               
               
               if ( currentSLittle > (oldSLittle * 1.05) or currentSLittle > (oldSLittle * 0.9)  )
               {
                   if (currentSLittle > (oldSLittle * 1.05) )
                   {
                       putText(img, "shocked", Point2f(300,300), FONT_HERSHEY_DUPLEX, 3,  Scalar(0,0,255,255));
                       
                   }
                   
                 //  else
                   if (currentSLittle > (oldSLittle * 0.9))
                   {
                       putText(img, "ANGRY", Point2f(300,300), FONT_HERSHEY_DUPLEX, 3,  Scalar(0,0,255,255));
                   }
               }
         
            }
        }*/
        
        int positionPointNose =  ( ((pos[2].x - pos[0].x) * (pos[1].y - pos[0].y)) - ((pos[2].y - pos[0].y) * (pos[1].x - pos[0].x)) ) ;
        int positionPointLip =  ( ((pos[3].x - pos[0].x) * (pos[1].y - pos[0].y)) - ((pos[3].y - pos[0].y) * (pos[1].x - pos[0].x)) ) ;
        
        if ( positionPointLip * positionPointNose > 0 ) // определение улыбки
        {
            putText(img, "NORMAL", Point2f(100,100), FONT_HERSHEY_DUPLEX, 3,  Scalar(0,0,255,255));
        } else {
            putText(img, "HAPPY", Point2f(100,100), FONT_HERSHEY_DUPLEX, 3,  Scalar(0,0,255,255));
        }
        
        imshow("", img);
    }
    
    oldSBig = getsqr ( newPositionPoint[0][0],newPositionPoint[0][16],newPositionPoint[0][8] );
    oldSLittle = getsqr ( newPositionPoint[0][27],newPositionPoint[0][21],newPositionPoint[0][22] );
    
    return 0;
}

vector < vector<Point2f> > pointAveraging ( vector < vector<Point2f> > point ) // усреднение позиций точек
{
    if (!start) {
        newPositionPoint = point;
        start = true;
        ++countFrame;
    } else {
        for(int i = 0; i < countPointLandmarsk; i++)
        {
            newPositionPoint[0][i].x += point[0][i].x;
            newPositionPoint[0][i].y += point[0][i].y;
        }
        ++countFrame;
        if (countFrame == 2)
        {
            for(int i = 0; i < countPointLandmarsk; i++)
            {
                newPositionPoint[0][i].x = newPositionPoint[0][i].x / 2;
                newPositionPoint[0][i].y = newPositionPoint[0][i].y / 2;
            }
            for(int i = 0; i < countPointLandmarsk; i++)
            {
                area();
            }
            countFrame = 0;
        }
    }
    return point;
}

int main()
{
    CascadeClassifier
    faceDetector("/UserPath/haarcascade_frontalface_alt_tree.xml");
    Ptr<Facemark> facemark = FacemarkLBF::create();
    facemark->loadModel("/UserPath/lbfmodel.yaml");
    
    VideoCapture cam(0);
    Mat frame,
    gray;
    while(cam.read(frame))
    {
        vector<Rect> faces;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        faceDetector.detectMultiScale(gray, faces);
        vector < vector<Point2f> > landmarks;
        vector < vector<Point2f> > contours;
        
        bool success = facemark -> fit(frame,faces,landmarks);
        if(success)
        {
            pointAveraging (landmarks);
        }
        if (waitKey(1) == 27) break; // esc
    }
    return 0;
}
