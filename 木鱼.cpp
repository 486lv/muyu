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
    
    loadimage(&b1, "./ͼƬ.png", 384, 250);

    putimage(80, 350, &b1);

    MOUSEMSG msg;
    int i = 0;
    int sum1 = 0; 
       int sum2 = 0;
       int sum3 = 0;
    srand((unsigned int)time(NULL));



    cout << "��ѡ��ģʽ" << endl;
    cout << "1.�ֶ�ģʽ 2.�Զ�ģʽ" << endl;
    int os;
    cin >> os;
    if (os == 1)
    {
        while (1)
    { 
        

        ExMessage m;//����һ����Ϣ����
        m = getmessage(EM_MOUSE);//��ȡ�����Ϣ 
        
        switch (m.message)
        {
     

        case WM_LBUTTONDOWN:
            
            //printf("���꣨%d��%d��\n", m.x, m.y);
            i++;
            sum1 +=rand()%11+0;
        
                
            outtextxy(m.x, m.y, "����+N");
            music();
            system("cls");
            cout << "�ܹ����ǣ�" << sum1;
            cout << "\r";
            
            break;


        case WM_RBUTTONUP:
           
            outtextxy(m.x, m.y, "�ǻ�+N");
            music();
            //printf("���꣨%d��%d��\n", m.x, m.y);
            system("cls");
             i++;
            sum2 += rand() % 11 + 0;
            cout << "���ǻ��ǣ�" << sum2;
            cout << "\r";
            break;
        case WM_MOUSEWHEEL:
            outtextxy(m.x, m.y, "����+N");
            music();
            i++;
            sum3 += rand() % 11 + 0;
            cout << "�������ǣ�" << sum3;
            cout << "\r";
            break;
        //case WM_MOUSEMOVE://���ƶ�������
        //    setfillcolor(WHITE);
        //    solidcircle(m.x, m.y, 2);
            break;
        case WM_MBUTTONDOWN:
            cout << "�ܹ����ǣ�" << sum1 << endl << "���ǻ��ǣ�" << sum2 << endl<< "�������ǣ�" << sum3;
            cout << "������ֵ��" << sum1 + sum2 - sum3 << endl;
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
            cout << "��������ľ�����";

            init();
        case 43:
          
            system("cls");
            cout << "����Բ�����õ�����";
           
            exit(0);
       
        case 64:
          
            system("cls");
            cout << "�ɽ���";
            exit(0);
        case 128:
         
            system("cls");
            cout << "��ȥ";
            exit(0);
            
            return;
        }


        switch (sum2)
        {
        case 43:

            system("cls");
            cout << "�����۷�";

            exit(0);

        case 64:

            system("cls");
            cout << "����";
            exit(0);
        case 128:

            system("cls");
            cout << "���Ȳ�����";
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
            ExMessage m;//����һ����Ϣ����
            m = getmessage(EM_MOUSE);//��ȡ�����Ϣ
            switch (style)
            {
            case 0:
                outtextxy(x, y, "����+N");
                i++;
                sum1 += rand() % 11 + 0;
                system("cls");
                cout << "�ܹ����ǣ�" << sum1;
                cout << "\r";
                break;
            case 1:
                outtextxy(x, y, "�ǻ�+N");
                i++;
                sum2 += rand() % 11 + 0;
                system("cls");
                cout << "���ǻ��ǣ�" << sum2;
                cout << "\r";
                break;
            case 2:
                outtextxy(x, y, "����+N");
                i++;
                sum3 += rand() % 11 + 0;
                system("cls");
                cout << "�������ǣ�" << sum3;
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
                cout << "�ܹ����ǣ�" << sum1 << endl << "���ǻ��ǣ�" << sum2 << endl << "�������ǣ�" << sum3 <<endl;
                cout <<"������ֵ��" << sum1 + sum2 - sum3 << endl;
                return;
            }
           
        }
    }
    else
    {
        cout << "����" << endl;
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