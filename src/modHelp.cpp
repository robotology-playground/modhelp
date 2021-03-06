/*
 * Copyright (C) 2012 MACSI - 2013 CODYCO
 * Author: Serena Ivaldi, MingXing Liu
 * email:   serena.ivaldi@isir.upmc.fr
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 */

#include <modHelp/modHelp.h>
//#include <yarp/os/all.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
//#include <yarp/dev/PolyDriver.h>
//#include <yarp/dev/Drivers.h>
//#include <yarp/dev/ControlBoardInterfaces.h>
//#include <yarp/dev/CartesianControl.h>
//#include <deque>
#include <iCub/ctrl/math.h>
#include <limits.h>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <iomanip>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace iCub::ctrl;
using namespace modHelp;


namespace modHelp{
    
    //================================
    //
    //	MODULE HELPERS
    //
    //================================
    
    
    //---------------------------------------------------------
    void readString(ResourceFinder &rf, string name, string &v, string vdefault)
    {
        if(rf.check(name.c_str()))
        {
            v = rf.find(name.c_str()).asString();
        }
        else
        {
            v = vdefault;
            cout<<"Could not find value for "<<name<<". "
            <<"Setting default "<<vdefault<<endl;
        }
        displayNameValue(name,v);
    }
    
    //---------------------------------------------------------
    void readDouble(ResourceFinder &rf, string name, double &v, double vdefault)
    {
        if(rf.check(name.c_str()))
        {
            v = rf.find(name.c_str()).asDouble();
        }
        else
        {
            v = vdefault;
            cout<<"Could not find value for "<<name<<". "
            <<"Setting default "<<vdefault<<endl;
        }
        displayNameValue(name,v);
    }
    
    //---------------------------------------------------------
    void readInt(ResourceFinder &rf, string name, int &v, int vdefault)
    {
        if(rf.check(name.c_str()))
        {
            v = rf.find(name.c_str()).asInt();
        }
        else
        {
            v = vdefault;
            cout<<"Could not find value for "<<name<<". "
            <<"Setting default "<<vdefault<<endl;
        }
        displayNameValue(name,v);
    }
    
    //---------------------------------------------------------
    void readBool(ResourceFinder &rf, string name, bool &v, bool vdefault)
    {
        if(rf.check(name.c_str()))
        {
            if((rf.find(name.c_str()).asString()=="true")||
               (rf.find(name.c_str()).asString()=="yes")||
               (rf.find(name.c_str()).asString()=="active")||
               (rf.find(name.c_str()).asString()=="on"))
                v = true;
            else
                v = false;
        }
        else
        {
            v = vdefault;
            cout<<"Could not find value true/false for "<<name<<". "
            <<"Setting default "<<((vdefault==true)?"true":"false")<<endl;
        }
        displayNameValue(name,((v==true)?"true":"false"));
    }
    
    //---------------------------------------------------------
    void readVector(ResourceFinder &rf, string name, Vector &v, int len)
    {
        v.resize(len,0.0);
        if(rf.check(name.c_str()))
        {
            Bottle &grp = rf.findGroup(name.c_str());
            for (int i=0; i<len; i++)
                v[i]=grp.get(1+i).asDouble();
        }
        else
        {
            cout<<"Could not find parameters for "<<name<<". "
            <<"Setting everything to zero by default"<<endl;
        }
        displayNameVector(name,v);
    }
    
    //---------------------------------------------------------
    void readString(Bottle &rf, string name, string &v, string vdefault)
    {
        if(rf.check(name.c_str()))
        {
            v = rf.find(name.c_str()).asString();
        }
        else
        {
            v = vdefault;
            cout<<"Could not find value for "<<name<<". "
            <<"Setting default "<<vdefault<<endl;
        }
        displayNameValue(name,v);
    }
    
    //---------------------------------------------------------
    void readDouble(Bottle &rf, string name, double &v, double vdefault)
    {
        if(rf.check(name.c_str()))
        {
            v = rf.find(name.c_str()).asDouble();
        }
        else
        {
            v = vdefault;
            cout<<"Could not find value for "<<name<<". "
            <<"Setting default "<<vdefault<<endl;
        }
        displayNameValue(name,v);
    }
    
    //---------------------------------------------------------
    void readInt(Bottle &rf, string name, int &v, int vdefault)
    {
        if(rf.check(name.c_str()))
        {
            v = rf.find(name.c_str()).asInt();
        }
        else
        {
            v = vdefault;
            cout<<"Could not find value for "<<name<<". "
            <<"Setting default "<<vdefault<<endl;
        }
        displayNameValue(name,v);
    }
    
    //---------------------------------------------------------
    void readBool(Bottle &rf, string name, bool &v, bool vdefault)
    {
        if(rf.check(name.c_str()))
        {
            if((rf.find(name.c_str()).asString()=="true")||(rf.find(name.c_str()).asString()=="on"))
                v = true;
            else
                v = false;
        }
        else
        {
            v = vdefault;
            cout<<"Could not find value true/false for "<<name<<". "
            <<"Setting default "<<((vdefault==true)?"true":"false")<<endl;
        }
        displayNameValue(name,((v==true)?"true":"false"));
    }
    
    //---------------------------------------------------------
    void readVector(Bottle &rf, string name, Vector &v, int len)
    {
        v.resize(len,0.0);
        if(rf.check(name.c_str()))
        {
            Bottle &grp = rf.findGroup(name.c_str());
            for (int i=0; i<len; i++)
                v[i]=grp.get(1+i).asDouble();
        }
        else
        {
            cout<<"Could not find parameters for "<<name<<". "
            <<"Setting everything to zero by default"<<endl;
        }
        displayNameVector(name,v);
    }
    
    
    
    
    //---------------------------------------------------------
    bool createDriver(PolyDriver *&p, Property &options)
    {
        //remove previously existing drivers if it exists
        if (p) { p->close(); delete p; p=0;}
        
        //creates the new device driver
        p = new PolyDriver(options);
        
        if(!p->isValid())
        {
            cout<<"Problems with the validity of the remote driver"<<endl;
            return false;
        }
        
        return true;
    }
    //---------------------------------------------------------
    void deleteDriver(PolyDriver *p)
    {
        if(p) {p->close(); delete p; p=0;}
    }
    //---------------------------------------------------------
    void deleteInterfaceEncoders(IEncoders *ie)
    {
        if(ie) {delete ie; ie=0;}
    }
    //---------------------------------------------------------
    void deleteInterfacePosition(IPositionControl *ip)
    {
        if(ip) {delete ip; ip=0;}
    }
    //---------------------------------------------------------
    void deleteInterfaceImpedance(IImpedanceControl *ii)
    {
        if(ii) {delete ii; ii=0;}
    }
    //---------------------------------------------------------
    void deleteInterfaceControl(IControlMode *ic)
    {
        if(ic) {delete ic; ic=0;}
    }
    
    //---------------------------------------------------------
    void fflushCin()
    {
        //cin.ignore(INT_MAX);
        cin.clear();
    }
    
    //---------------------------------------------------------
    bool connectPorts(const string &portFrom, const string &portTo, int retry)
    {
        bool connected = false; int count=0;
        
        cout<<" +++ Connecting: "<<portFrom<<" => "<<portTo<<" ";
        while((connected==false)&&(count<retry))
        {
            connected = Network::connect(portFrom.c_str(),portTo.c_str());
            if(connected==false)
            {
                if(count==retry)
                {
                    cout<<": failed => ERROR "<<endl;
                    return false;
                }
                
                cout<<": retrying"<<endl;
                Time::delay(0.5);
                count++;
            }
            else
                cout<<": done"<<endl;
        }
        return true;
    }
    
    //---------------------------------------------------------
    void closePort(Contactable *port)
    {
        if(port)
        {
            port->interrupt();
            port->close();
            
            delete port;
            port = 0;
        }
    }
    
    //---------------------------------------------------------
    string toString(bool val, int type)
    {
        string ret="";
        
        if(val==false)
        {
            switch(type)
            {
                case 1: ret="false"; break;
                case 2: ret="off"; break;
                case 3: ret="no"; break;
                case 4: ret="disabled"; break;
                default: ret="0";
            }
        }
        else
        {
            switch(type)
            {
                case 1: ret="true"; break;
                case 2: ret="on"; break;
                case 3: ret="yes"; break;
                case 4: ret="enabled"; break;
                default: ret="1";
            }
        }
        return ret;
    }
    
    //---------------------------------------------------------
    bool yarpToEigenVector(const yarp::sig::Vector &yarpVector, Eigen::VectorXd &eigenVector)
    {
        if(yarpVector.size() == 0)
        {
            cout<<"ERROR: input vector is empty (yarpToEigenVector)"<<endl;
            return false;
        }

        //resize and fill eigen vector with yarp vector elements
        eigenVector.resize(yarpVector.size());
        for(unsigned int i = 0; i < yarpVector.size(); i++)
            eigenVector(i) = yarpVector(i);

        return true;
    };

    //---------------------------------------------------------
    bool eigenToYarpVector(const Eigen::VectorXd &eigenVector, yarp::sig::Vector &yarpVector)
    {
        if(eigenVector.size() == 0)
        {
            cout<<"ERROR: input vector is empty (eigenToYarpVector)"<<endl;
            return false;
        }

        //resize and fill eigen vector with yarp vector elements
        yarpVector.resize(eigenVector.size());
        for(unsigned int i=0; i <eigenVector.size(); i++)
                yarpVector(i) = eigenVector(i);

        return true;
    };

    //---------------------------------------------------------
    bool yarpToEigenMatrix(const yarp::sig::Matrix& yarpMatrix, Eigen::MatrixXd& eigenMatrix)
    {
        if((eigenMatrix.cols() == 0)||(eigenMatrix.rows() == 0))
        {
            cout<<"ERROR: input matrix is empty (yarpToEigenMatrix)"<<endl;
            return false;
        }

        //resize and fill eigen vector with yarp vector elements
        eigenMatrix.resize(yarpMatrix.rows(),yarpMatrix.cols());
        
        for(unsigned int i = 0; i < eigenMatrix.rows(); i++)
            for(unsigned int j = 0; j < eigenMatrix.cols(); j++)
                eigenMatrix(i,j) = yarpMatrix(i,j);

        return true;
    };

    //---------------------------------------------------------
    bool eigenToYarpMatrix(const Eigen::MatrixXd& eigenMatrix, yarp::sig::Matrix& yarpMatrix)
    {
        if((yarpMatrix.cols() == 0)||(yarpMatrix.rows() == 0))
        {
            cout<<"ERROR: input matrix is empty (eigenToYarpMatrix)"<<endl;
            return false;
        }

        //resize and fill eigen vector with yarp vector elements
        yarpMatrix.resize(eigenMatrix.rows(),eigenMatrix.cols());
        
        for(unsigned int i = 0; i < yarpMatrix.rows(); i++)
            for(unsigned int j = 0; j < yarpMatrix.cols(); j++)
                yarpMatrix(i,j) = eigenMatrix(i,j);
            
        return true;
    };
    
}//namespace modHelp

