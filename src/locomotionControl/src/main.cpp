#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <iCub/ctrl/math.h>
#include <yarp/math/Math.h>

#include <string.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <locomotion\locomotionModule.h>


using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace std;
using namespace locomotion;


int main (int argc, char * argv[])
{
    //Creating and preparing the Resource Finder
    ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("default.ini");         //default config file name.
    rf.setDefaultContext("locomotionControl/conf"); //when no parameters are given to the module this is the default context    
    rf.configure("ICUB_ROOT",argc,argv);
    // rf.setName("locomotionControl");

    if (rf.check("help"))
    {
        cout<< "Possible parameters"                                                                                                                                          << endl << endl;
        cout<< "\t--context          :Where to find an user defined .ini file within $ICUB_ROOT/app e.g. /locomotionCtrl/conf"                                   <<endl;
        cout<< "\t--from             :Name of the file.ini to be used for calibration."                                                                                       <<endl;
        cout<< "\t--rate             :Period used by the module. Default set to 10ms."                                                                                        <<endl;
        cout<< "\t--robot            :Robot name (icubSim or icub). Set to icub by default."                                                                                  <<endl;
        cout<< "\t--local            :Prefix of the ports opened by the module. Set to the module name by default, i.e. locomotionCtrl."                                      <<endl;        
        return 0;
    }
    
    Network yarp;

    if (!yarp.checkNetwork())
    {
        fprintf(stderr,"Sorry YARP network is not available\n");
        return -1;
    }

    //Creating the module
    LocomotionModule module;
    return module.runModule(rf);
}
