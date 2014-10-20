#include "solver.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"

int *combiA;
int combiC;
int combiN, combiK;
int count;
int * m;
float * x;
float * xx;
float ** mm;
float * v;
int n, solutions;

void p()
{
        int i, j;
        printf("=====\n");
        for (i = 0; i < n; i++){
                for (j = 0; j <= n; j++)
                        printf("%f ", mm[i][j]);
                puts("");
        }
}

void byteFieldToArray()
{
        int i, j;
        for (i = 0; i < n; i++)
        {
                for (j = 0; j < n; j++)
                {
                        if(m[i] & 1 << (n-j-1)) mm[i][n-j-1] = 1;
                        else  mm[i][n-j-1] = 0;
                }
                mm[i][n] = v[m[i] - 1];
        }
}

int gauss()
{
        int i, j, k, t;
        float buf;
        for (i = 0; i < n - 1; i++)
        {
                t = 0;
                //ищем подходящую строку для замены
                if(fabs(mm[i][i]) <= 0.000001)
                {
                        t = 1;
                        for (j = i + 1; j < n; j++)
                        {
                                if(fabs(mm[j][i]) > 0.000001)
                                {
                                        for (k = 0; k < n + 1; k++)
                                                buf = mm[j][k], mm[j][k] = mm[i][k], mm[i][k] = buf;
                                        t = 0;
                                        break;
                                }
                        }
                }
                if(t) return 0;
                for (j = i + 1; j < n; j++)
                {
                        if(mm[j][i] != 0.0)
                        {
                                buf = mm[j][i] / mm[i][i];
                                mm[j][i] = 0;
                                for (k = i + 1; k < n + 1; k++)
                                        mm[j][k] -= buf*mm[i][k];
                        }
                }
        }
        if(fabs(mm[n-1][n-1]) < 0.000001) return 0;
        for (i = n - 1; i >= 0; i--)
        {
                buf = mm[i][n];
                for (j = i + 1; j < n; j++)
                {
                        buf -= mm[i][j]*x[j];
                }
                x[i] = buf / mm[i][i];
        }
        return 1;
}

//пробегаем по всем коалициям и смотрим, чтобы решение было больше этой коалиции, иначе оно несправедливо
int check_C()
{
    int i, j;
    float buf;
    for (i = 0; i < (1 << n) - 1; i++)
    {
        buf = 0;
        for(j = 0; j < n; j++)
        {
            if((i+1) & (1 << j)) //но работает!
                buf += x[j];
        }
        if(buf < v[i]) return 0;
    }
    return 1;
}

void Solver::combiStep(int pos)
{
    int i;
    if(combiC==combiK)
    {
            int buf = (int)((float)(combiA[0])/((float)((1<<n)-2))*100.0); //lisp
            if(buf > count)
            {
                count = buf;
                progress(count);
            }
            for (i = 0; i < n - 1; i++)
                    m[i] = combiA[i] + 1;
            m[n-1] = (1 << n) - 1;
            byteFieldToArray();
            //отсеиваем решения
            if(gauss())
            {
                //отсеиваем решения по принципу a[i] > v[i]
                if(check_C())
                {
                    solutions++;
                    //p();
                   // printf("Solution!!!\n");
                    QString s = "";
                    for (i = 0; i < n; i++)
                    {
                        xx[i] += x[i];
                        s += QString::number(x[i]) + " ";
                        //printf("%f ", x[i]);
                    }
                    sendText(s);
                    //puts("");
                }
            }
            return;
    }
    if(pos + combiK - combiC > combiN)
            return;
    combiC++;
    for(i = pos ? combiA[pos-1] + 1 : 0 ; i < combiN ; i++)
    {
            combiA[combiC-1] = i;
            Solver::combiStep(pos+1);
    }
    combiC--;
}

void Solver::combinate(unsigned int N, unsigned int K)
{
        int i;
        combiN = N;
        combiK = K;
        combiA = new int [N];
        for(i = 0; i < combiN; i++)
                combiA[i] = i;
        combiStep(0);
        delete combiA;
}


void Solver::run()
{
    n = spin->value();
    if(n > 14) n = 14;
    if(n < 1) n = 1;
    solutions = 0;
    sendText(QString::fromUtf8("Ищу решения..."));
    count = 0;

    int i;
    mm = new float*[n];
    m = new int[n];
    x = new float[n];
    xx = new float[n];
    v = new float[(1 << n) - 1];

    for(i = 0; i < (1 << n) - 1; i++)
        v[i] = table->item(i,1)->text().toFloat();

    for(i = 0; i < n; i++)
    {
        x[i] = 0.0f;
        mm[i] = new float[n+1];
    }

    combinate((1 << n) - 2, n - 1);

    if(solutions)
    {
        QString s = "";
        for(i = 0; i < n; i++)
        {
            xx[i] /= (float)solutions;
            s += QString::number(xx[i]) + " ";
        }
        sendText(QString::fromUtf8("Решения найдены, среднее: ") + s);
    }
    else
        sendText(QString::fromUtf8("Решения не найдены!"));

    for(i = 0; i < n; i++)
        delete mm[i];
    delete mm;
    delete x;
    delete xx;
    delete m;
    delete v;
}


