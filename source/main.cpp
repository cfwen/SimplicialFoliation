#include "SimplicialFoliation.h"
#include "SimplicialFoliation3.h"


int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        cout << "usage: SimplicialFoliation.exe [3D] mesh output-mesh" << endl;
        return 0;
    }

    string arg1(argv[1]);
    if (arg1 == string("3D") || arg1 == string("3d"))
    {
        if (argc < 4)
        {
            cout << "usage: SimplicialFoliation.exe 3D mesh output-mesh" << endl;
            return 0;
        }
        CSimplicialFoliation3 sf;
        string filename(argv[2]);
        sf.readMesh(filename);
        sf.calculateFoliationDirectionField();

        string outfilename(argv[3]);
        sf.output(outfilename);
    }
    else
    {
        CSimplicialFoliation sf;
        string filename(argv[1]);
        sf.readMesh(filename);
        sf.calculateFoliationDirectionField();

        string outfilename(argv[2]);
        sf.output(outfilename);
    }
    return 0;
}
