#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
using namespace cv;


double*** activation(int channel, int inputsize1, int inputsize2, int ***input)
{
    //memory value
    double ***act_output;
    //memory assign
    act_output=(double***)malloc(channel*sizeof(double**));
    for(int i=0;i<channel;i++)
    {
	    *(act_output+i)=(double**)malloc(inputsize1*sizeof(double*));
	    for(int j=0;j<inputsize1;j++)
        {
            *(*(act_output+i)+j)=(double*)malloc(inputsize2*sizeof(double));
        }
    }
    //activation
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<inputsize1;x++)
        {
            for(int y=0;y<inputsize2;y++)
            {
                   act_output[n][x][y]=1/(1+exp(-input[n][x][y]));            
            }
        } 
    }

    return act_output;
}


int*** max_pooling(int channel, int inputsize1, int inputsize2, int ***input, int *mp_outputsize1, int *mp_outputsize2)
{
    //memory value
    int ***mp_output;
    int mp_filtersize;
    int mp_s;
    //enter value
    cout<<"max pooling filter size: ";
    cin>>mp_filtersize;
    cout<<"max pooling stride: ";
    cin>>mp_s;
    *mp_outputsize1=(inputsize1-mp_filtersize)/mp_s+1;
    *mp_outputsize2=(inputsize2-mp_filtersize)/mp_s+1;
    //memory assign
    mp_output=(int***)malloc(channel*sizeof(int**));
    for(int i=0;i<channel;i++)
    {
	    *(mp_output+i)=(int**)malloc(*mp_outputsize1*sizeof(int*));
	    for(int j=0;j<*mp_outputsize1;j++)
        {
            *(*(mp_output+i)+j)=(int*)malloc(*mp_outputsize2*sizeof(int));
        }
    }
    //pooling
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<*mp_outputsize1;x++)
        {
            for(int y=0;y<*mp_outputsize2;y++)
            {
                for(int i=0;i<mp_filtersize;i++)
                {
                    for(int j=0;j<mp_filtersize;j++)
                    {
                        if(mp_output[n][x][y]<input[n][i+mp_s*x][j+mp_s*y])
                        {
                            mp_output[n][x][y]=input[n][i+mp_s*x][j+mp_s*y];
                        }
                    }
                }
            }
        } 
    }
    return mp_output;
}


int*** aver_pooling(int channel, int inputsize1, int inputsize2, int ***input, int *ap_outputsize1, int *ap_outputsize2)
{
    //memory value
    int ***ap_output;
    int ap_filtersize;
    int ap_s;
    //enter value
    cout<<"average pooling filter size: ";
    cin>>ap_filtersize;
    cout<<"average pooling stride: ";
    cin>>ap_s;
    *ap_outputsize1=(inputsize1-ap_filtersize)/ap_s+1;
    *ap_outputsize2=(inputsize2-ap_filtersize)/ap_s+1;
    //memory assign
    ap_output=(int***)malloc(channel*sizeof(int**));
    for(int i=0;i<channel;i++)
    {
	    *(ap_output+i)=(int**)malloc(*ap_outputsize1*sizeof(int*));
	    for(int j=0;j<*ap_outputsize1;j++)
        {
            *(*(ap_output+i)+j)=(int*)malloc(*ap_outputsize2*sizeof(int));
        }
    }
    //pooling
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<*ap_outputsize1;x++)
        {
            for(int y=0;y<*ap_outputsize2;y++)
            {
                for(int i=0;i<ap_filtersize;i++)
                {
                    for(int j=0;j<ap_filtersize;j++)
                    {
                        ap_output[n][x][y]+=input[n][i+ap_s*x][j+ap_s*y];
                    }
                }
                if(((ap_output[n][x][y]%(ap_filtersize*ap_filtersize))*10)/(ap_filtersize*ap_filtersize)>=5)
                {
                    ap_output[n][x][y]/=(ap_filtersize*ap_filtersize);
                    ap_output[n][x][y]+=1;
                }
                else
                {
                    ap_output[n][x][y]/=(ap_filtersize*ap_filtersize);
                }
            
            }
        } 
    }

    return ap_output;
}


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
    int ***out;
    int outsize1;
    int outsize2;
    //fuction call
    int f;
    cout<<"select fuction (1:convolution 2:max pooling 3:average pooling): ";
    cin>>f;
    if(f==1)
    {
        out=conv(channel, inputsize1, inputsize2, input, &outsize1, &outsize2);
    }
    else if(f==2)
    {
        out=max_pooling(channel, inputsize1, inputsize2, input, &outsize1, &outsize2);
    }
    else if(f==3)
    {
        out=aver_pooling(channel, inputsize1, inputsize2, input, &outsize1, &outsize2);
    }
    else
    {
        cout<<"error"<<endl;
    }
    
    //new_image 
    Mat new_image(outsize1,outsize2,CV_8UC3,Scalar(0,1,2));
    //enter output in new_image
    for(int n=0;n<channel;n++)
    {
        for(int x=0;x<new_image.rows;x++)
        {
            for(int y=0;y<new_image.cols;y++)
            {
                new_image.at<cv::Vec3b>(x,y)[n]=out[n][x][y];
            }
        }
    }
    //image write
    imwrite("pooling_new_image2.jpg",new_image);
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
            free(*(*(out+i)+j));
        }
        free(*(out+i));
    }
    free(out);
   
    return 0;
}