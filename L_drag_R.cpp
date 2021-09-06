#pragma once
#include "stdafx.h"
#include "SCServo/SCServo.h"
#include <iostream>

using namespace std;

SMSBL sm;

int p_R[8] = { 2047,2047,2047,2047,2047,2047,2047,2047 };
int p_L[8] = { 2047,2047,2047,2047,2047,2047,2047,2047 };

int a_R[8] = { 0,90,90,0,0,90,0,20 };
int a_L[8] = { 0,90,90,0,0,90,0,20 };

int map_R[8][4] = { {-90,90,3071,1023},
                               {0,100,1023,2161},
                               {0,180,3071,1023},
                               {0,125,2047,625},
                               {-90,90,3071,1023},
                               {70,110,1477,2617},
                               {-75,75,2900,1194},
                               {0,90,2160,1510} };
int map_L[8][4] = { {-90,90,1023,3071},
                              {80,180,1933,3071},
                              {0,180,3071,1023},
                              {0,125,2047,625},
                              {-90,90,3071,1023},
                              {70,110,1477,2617},
                              {-75,75,2900,1194},
                              {0,90,1950,2600} };

int v = 500;                 //舵机转速、加速度
int a = 50;

float map(float num, float s1min, float s1max, float s2min, float s2max)            //把num从s1范围映射到s2范围
{
    float num1;
    num1 = (num - s1min) * (s2max - s2min) / (s1max - s1min) + s2min;
    return num1;
}

int insure(int p[], int isRL)                   //判定是否超出舵机转角范围
{
    int cnt = 0;
    for (int i = 0; i < 8; i++)
    {
        if (abs(p[i] - (map_R[i][2] + map_R[i][3]) / 2) < abs((map_R[i][2] - map_R[i][3]) / 2) + 2)
        {
            cnt += pow(10, i);
        }
    }
    if (cnt == 11111111)
        return 1;
    else
    {
        cout << cnt << endl;
        return 0;
    }
}


int main(int argc, char** argv)
{
    if (argc < 2) {                  //检验是否设置端口
        cout << "argc error!" << endl;
        getchar();
        return 0;
    }
    cout << "serial:" << argv[1] << endl;
    if (!sm.begin(115200, argv[1])) {                  //检验波特率及端口是否正确
        cout << "Failed to init smb motor!" << endl;
        getchar();
        return 0;
    }
    //================================以上为初始化================================

    for (int i = 0; i < 8; i++)
    {
        if (i == 5) sm.WritePosEx(1 + i, p_R[i], 2 * v, 2 * a);
        else sm.WritePosEx(1 + i, p_R[i], v, a);
    }

    while (true)
    {
        for (int i = 0; i < 8; i++)
        {
            p_L[i] = sm.ReadPos(11 + i);
            
            a_L[i] = int(map(p_L[i], map_L[i][2], map_L[i][3], map_L[i][0], map_L[i][1]));

            switch (i)
            {
            case 0: case 3: case 5: case 7:
                a_R[i] = a_L[i];
                break;
            case 1: case 2:
                a_R[i] = 180 - a_L[i];
                break;
            case 4: case 6:
                a_R[i] = - a_L[i];
                break;
            default:
                break;
            }

            p_R[i] = int(map(a_R[i], map_R[i][0], map_R[i][1], map_R[i][2], map_R[i][3]));
        }

        if (insure(p_R, 1))
        {
            for (int i = 0; i < 8; i ++)
            {
                if (i == 5) sm.WritePosEx(1 + i, p_R[i], 2 * v, 2 * a);
                else sm.WritePosEx(1 + i, p_R[i], v, a);
            }
        }
        Sleep(10);
    }
}
