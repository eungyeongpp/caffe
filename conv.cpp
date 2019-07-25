#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
using namespace cv;


int*** conv(int channel, int inputsize1, int inputsize2, int ***input, int *outputsize1, int *outputsize2)
{
    //memory value
    int ***output, ***padding;
    double **filter;
    int filtersize, paddingsize1, paddingsize2;
    int p,s;
    cout<<"filter size: ";
    cin>>filtersize;
    cout<<"padding: ";
    cin>>p;
    cout<<"stride: ";
    cin>>s;
    *outputsize1=(inputsize1-filtersize+2*p)/s+1;
    *outputsize2=(inputsize2-filtersize+2*p)/s+1;
    paddingsize1=inputsize1+p*2;  
    paddingsize2=inputsize2+p*2; 
    //memory assign
    filter=(double**)malloc(filtersize*sizeof(double*));
    output=(int***)malloc(channel*sizeof(int**));
    padding=(int***)malloc(channel*sizeof(int**));    
    for(int i=0;i<filtersize;i++)
    {
        *(filter+i)=(double*)malloc(filtersize*sizeof(double));
    }
    for(int i=0;i<channel;i++)
    {
	    *(output+i)=(int**)malloc(*outputsize1*sizeof(int*));
	    for(int j=0;j<*outputsize1;j++)
        {
            *(*(output+i)+j)=(int*)malloc(*outputsize2*sizeof(int));
        }
    }
    for(int i=0;i<channel;i++)
    {
	    *(padding+i)=(int**)malloc(paddingsize1*sizeof(int*));
	    for(int j=0;j<paddingsize1;j++)
        {
            *(*(padding+i)+j)=(int*)malloc(paddingsize2*sizeof(int));
        }
    }
    //padding initialize
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<(paddingsize1);x++)
        {   
            for(int y=0;y<(paddingsize2);y++)
            {
                padding[n][x][y]=0;
            }
        }
    }
    //enter input in padding
    for(int n=0;n<(channel);n++)
    {
        for(int x=p;x<(inputsize1+p);x++)
        {
            for(int y=p;y<(inputsize2+p);y++)
            {
                padding[n][x][y]=input[n][x-p][y-p];
            }
        }
    }
    //enter filter
    double v_fil;
    cout<<endl<<"enter filter value"<<endl;
    for(int x=0;x<filtersize;x++)
    {
        for(int y=0;y<filtersize;y++)
        {
            cout<<"["<<x<<"]["<<y<<"]: ";
            cin>>v_fil;
            filter[x][y]=v_fil;
        }
        cout<<endl;
    }
    //convolution time check
    clock_t begin=clock();
    //convolution
    for(int n=0;n<(channel);n++)
    {
        for(int x=0;x<*outputsize1;x++)
        {
            for(int y=0;y<*outputsize2;y++)
            {
                for(int i=0;i<filtersize;i++)
                {
                    for(int j=0;j<filtersize;j++)
                    {
                        output[n][x][y]+=padding[n][i+s*x][j+s*y]*filter[i][j];
                    }
                }
            }
        } 
    }
    //convolution time check
    clock_t end=clock();
    double elapsed_secs=double(end-begin)/CLOCKS_PER_SEC;
    cout<<"convolution time: "<<elapsed_secs<<endl;
    //saturation
    for(int n=0;n<(channel);n++)
    {
        for(int x=0;x<*outputsize1;x++)
        {
            for(int y=0;y<*outputsize2;y++)
            {
                for(int i=0;i<filtersize;i++)
                {
                    for(int j=0;j<filtersize;j++)
                    {
                        if(output[n][x][y]>255)
                        {
                            output[n][x][y]=255;
                        }
                        else if(output[n][x][y]<0)
                        {
                            output[n][x][y]=0;
                        }
                    }
                }
            }
        } 
    }
    //free
    for(int i=0;i<channel;i++)
    {
        for(int j=0;j<paddingsize1;j++)
        {
            free(*(*(padding+i)+j));
        }
        free(*(padding+i));
    }
    free(padding);
    for(int i=0;i<filtersize;i++)
    {
        free(*(filter+i));
    }
    free(filter);

    return output;
}

Mat conv2( cv::Mat src, int kernel_size )
{
    cv::Mat dst,kernel;
    kernel = (Mat_<float>(kernel_size,kernel_size) << -1,-1,-1,-1,8,-1,-1,-1,-1);
  
    /// Apply filter
    cv::filter2D( src, dst, -1 , kernel, Point( -1, -1 ), 0, BORDER_CONSTANT );

    return dst;
}


int main()
{
    //image read
    Mat image;
    image=imread("umbrella.jpg",IMREAD_COLOR);
    if(image.empty())
    {
        cout<<"Cloud not open or find the image"<<endl;
        return -1;
    }
    //input memory value
    int channel=3;
    int ***input;
    int inputsize1=image.rows;
    int inputsize2=image.cols;
    //input memory assign
    input=(int***)malloc(channel*sizeof(int**));
    for(int i=0;i<channel;i++)
    {
	    *(input+i)=(int**)malloc(inputsize1*sizeof(int*));
	    for(int j=0;j<inputsize1;j++)
        {
            *(*(input+i)+j)=(int*)malloc(inputsize2*sizeof(int));
        }
    }
    //enter image in input
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<image.rows;x++)
        {
            for(int y=0;y<image.cols;y++)
            {
                input[n][x][y]=image.at<cv::Vec3b>(x,y)[n];
            }
        }
    }
    //output memory value
    int ***out1;
    int outsize1;
    int outsize2;
    Mat out2;
    //call function
    out1=conv(channel, inputsize1, inputsize2, input, &outsize1, &outsize2); 
    out2=conv2(image,3);
    //out_image 
    Mat out_image(outsize1,outsize2,CV_8UC3,Scalar(0,1,2));
    //enter out1 in out_image
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<out_image.rows;x++)
        {
            for(int y=0;y<out_image.cols;y++)
            {
                out_image.at<cv::Vec3b>(x,y)[n]=out1[n][x][y];
            }
        }
    }
    //comp_image
    Mat comp_image(outsize1,outsize2,CV_8UC3,Scalar(0,1,2));
    //enter sub in comp_image
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<comp_image.rows;x++)
        {
            for(int y=0;y<comp_image.cols;y++)
            {
                comp_image.at<cv::Vec3b>(x,y)[n]=out2.at<cv::Vec3b>(x,y)[n]-out_image.at<cv::Vec3b>(x,y)[n];
            }
        }
    }
    //image pixel sum
    float sum=0;
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<comp_image.rows;x++)
        {
            for(int y=0;y<comp_image.cols;y++)
            {
                sum+=comp_image.at<cv::Vec3b>(x,y)[n];
            }
        }
    }
    cout<<"image pixel sum: "<<sum<<endl;
    //image write
    imwrite("comp.jpg",comp_image);
    //input free
    for(int i=0;i<channel;i++)
    {
        for(int j=0;j<inputsize1;j++)
        {
            free(*(*(input+i)+j));
        }
        free(*(input+i));
    }
    free(input);
    //function free
    for(int i=0;i<channel;i++)
    {
        for(int j=0;j<outsize1;j++)
        {
            free(*(*(out1+i)+j));
        }
        free(*(out1+i));
    }
    free(out1);
   
    return 0;
}