#include <iostream>
using namespace std;
#include <graphics.h>
#include <easyx.h>
#include <string>
#include <stdlib.h >
#include "conio.h"
#include <ctime>
IMAGE b1;
IMAGE word;
#include<mmsystem.h>
#include <conio.h>
#include <mciapi.h>
#pragma comment(lib,"winmm.lib")


void music()
{
   
    
   
}
void init()
{
    
    initgraph(512, 720, SHOWCONSOLE);
    
    loadimage(&b1, "./图片.png", 384, 250);

    putimage(80, 350, &b1);

    MOUSEMSG msg;
    int i = 0;
    int sum1 = 0; 
       int sum2 = 0;
       int sum3 = 0;
    srand((unsigned int)time(NULL));



    cout << "请选择模式" << endl;
    cout << "1.手动模式 2.自动模式" << endl;
    int os;
    cin >> os;
    if (os == 1)
    {
        while (1)
    { 
        

        ExMessage m;//定义一个消息变量
        m = getmessage(EM_MOUSE);//获取鼠标消息 
        
        switch (m.message)
        {
     

        case WM_LBUTTONDOWN:
            
            //printf("坐标（%d，%d）\n", m.x, m.y);
            i++;
            sum1 +=rand()%11+0;
        
                
            outtextxy(m.x, m.y, "功德+N");
            music();
            system("cls");
            cout << "总功德是：" << sum1;
            cout << "\r";
            
            break;


        case WM_RBUTTONUP:
           
            outtextxy(m.x, m.y, "智慧+N");
            music();
            //printf("坐标（%d，%d）\n", m.x, m.y);
            system("cls");
             i++;
            sum2 += rand() % 11 + 0;
            cout << "总智慧是：" << sum2;
            cout << "\r";
            break;
        case WM_MOUSEWHEEL:
            outtextxy(m.x, m.y, "虚无+N");
            music();
            i++;
            sum3 += rand() % 11 + 0;
            cout << "总虚无是：" << sum3;
            cout << "\r";
            break;
        //case WM_MOUSEMOVE://在移动过程中
        //    setfillcolor(WHITE);
        //    solidcircle(m.x, m.y, 2);
            break;
        case WM_MBUTTONDOWN:
            cout << "总功德是：" << sum1 << endl << "总智慧是：" << sum2 << endl<< "总虚无是：" << sum3;
            cout << "您的正值：" << sum1 + sum2 - sum3 << endl;
            return;
        }

        if (i == 10)
        {
            i = 0;
            cleardevice();
            putimage(80, 350, &b1);

        }
        if (sum1 > 150 || sum2 > 150)
        {
            init();
        }
        switch (sum1)
        {
        case 27:
            system("cls");
            cout << "心无杂念，心境宁静";

            init();
        case 43:
          
            system("cls");
            cout << "功德圆满，得道修仙";
           
            exit(0);
       
        case 64:
          
            system("cls");
            cout << "渡劫期";
            exit(0);
        case 128:
         
            system("cls");
            cout << "逝去";
            exit(0);
            
            return;
        }


        switch (sum2)
        {
        case 43:

            system("cls");
            cout << "智力巅峰";

            exit(0);

        case 64:

            system("cls");
            cout << "弱智";
            exit(0);
        case 128:

            system("cls");
            cout << "是秩不是智";
            init();

            return;
        }
    }
    }
    else if (os == 2)
    {
        int i = 0;
        int sum1 = 0;
        int sum2 = 0;
        int sum3 = 0;
        int sum4 = 0;

        while (1)
        {
            int x=rand() % 513;
            int y=rand() % 721;
            int style = rand() % 4;
            ExMessage m;//定义一个消息变量
            m = getmessage(EM_MOUSE);//获取鼠标消息
            switch (style)
            {
            case 0:
                outtextxy(x, y, "功德+N");
                i++;
                sum1 += rand() % 11 + 0;
                system("cls");
                cout << "总功德是：" << sum1;
                cout << "\r";
                break;
            case 1:
                outtextxy(x, y, "智慧+N");
                i++;
                sum2 += rand() % 11 + 0;
                system("cls");
                cout << "总智慧是：" << sum2;
                cout << "\r";
                break;
            case 2:
                outtextxy(x, y, "虚无+N");
                i++;
                sum3 += rand() % 11 + 0;
                system("cls");
                cout << "总虚无是：" << sum3;
                cout << "\r";
                break;
           
                
         }
           Sleep(50);
            if (i == 50)
            {
                i = 0;
                cleardevice();
                putimage(80, 350, &b1);

            }
            
            if (m.message == WM_RBUTTONDOWN)
            {
                cout << "总功德是：" << sum1 << endl << "总智慧是：" << sum2 << endl << "总虚无是：" << sum3 <<endl;
                cout <<"您的正值：" << sum1 + sum2 - sum3 << endl;
                return;
            }
           
        }
    }
    else
    {
        cout << "错误" << endl;
        exit(0);
    }
}



int main()
{
  	
    //mciSendString("open res/muyu.mp3 alias mas", 0, 0, 0);

    music();
    mciSendString("open res/muyu.mp3 alias mas", 0, 0, 0);
    mciSendString("play mas", 0, 0, 0);
    mciSendString("play music repeat", 0, 0, 0);
    init();
    

	getchar();

	closegraph();
	return 0;
}