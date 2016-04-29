#include <iostream>
#include <fstream>
int main(int argc, char * argv[])
{
    std::fstream myfile("dist.txt", std::ios_base::in);
    std::ofstream myfile1;

    float a, b, c, d;
    float e[27],f[27],g[27],h[27];
    int i, j;
    myfile1.open ("final1.txt",std::ios_base::app);
    while (1)
    {
        for(i=0;i<27;i++)
        {
            h[i]=0;
        }
        for(j=0;j<3;j++)
        {
            for(i=0;i<27;i++)
            {
                myfile >> a >> b >> c >> d;
                if(myfile.eof())
                {
                    myfile1.close();
                    return 0;
                }
                e[i]=a;
                f[i]=b;
                g[i]=c;
                h[i]=h[i]+d;
            }
        }
        for(i=0;i<27;i++)
        {
            h[i]=h[i]/3;
            myfile1 << e[i] <<"\t"<<f[i]<<"\t"<<g[i]<<"\t"<<h[i]<<"\n";
        }

    }
    myfile1.close();
    return 0;
}
