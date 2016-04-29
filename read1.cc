#include <iostream>
#include <fstream>
int main(int argc, char * argv[])
{
    std::fstream myfile("through.txt", std::ios_base::in);
    std::ofstream myfile1;

    float a, b, c, d;
    float e[15],f[15],g[15],h[15];
    int i, j;
    myfile1.open ("final2.txt",std::ios_base::app);
    while (1)
    {
        for(i=0;i<15;i++)
        {
            g[i]=0;
            h[i]=0;
        }
        for(j=0;j<3;j++)
        {
            for(i=0;i<15;i++)
            {
                myfile >> a >> b >> c >> d;
                if(myfile.eof())
                {
                    myfile1.close();
                    return 0;
                }
                e[i]=a;
                f[i]=b;
                g[i]=g[i]+c;
                h[i]=h[i]+d;
            }
        }
        for(i=0;i<15;i++)
        {
            g[i]=g[i]/3;
            h[i]=h[i]/3;
            myfile1 << e[i] <<"\t"<<f[i]<<"\t"<<g[i]<<"\t"<<h[i]<<"\n";
        }

    }
    myfile1.close();
    return 0;
}
