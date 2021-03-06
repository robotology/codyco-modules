/*
 * Copyright (C) 2015 Robotics, Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Silvio Traversaro
 * email: silvio.traversaro@iit.it
 *
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef WBSTATESLOCAL_ICUB_H
#define WBSTATESLOCAL_ICUB_H

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IVelocityControl2.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/BufferedPort.h>
#include <iCub/ctrl/adaptWinPolyEstimator.h>
#include <iCub/ctrl/filters.h>
#include <iCub/iDynTree/TorqueEstimationTree.h>
#include <iCub/skinDynLib/skinContactList.h>
#include <yarpWholeBodyInterface/yarpWbiUtil.h>
#include <map>
#include <set>


#include "yarpWholeBodyInterface/yarpWholeBodySensors.h"

#include "wholeBodyDynamicsTree/robotStatus.h"


namespace wbi {
    class ID;
    class IDList;
}

using namespace yarpWbi;


    class TorqueEstimationSubtree
    {
    public:
        std::string subtree_name;
        std::vector<std::string> links;
        std::set<int> links_numeric_ids;
        int default_contact_link;
    };


    //< \todo TODO make SKIN_EVENTS_TIMEOUT a proper parameter
    #define SKIN_EVENTS_TIMEOUT 0.2     // max time (in sec) a contact is kept without reading anything from the skin events port
     /**
     * Thread that estimates the dynamic state of the iCub robot.
     */
    class ExternalWrenchesAndTorquesEstimator
    {
    protected:
        double periodInMilliSeconds;
        yarpWbi::yarpWholeBodySensors        *sensors;
        yarp::os::BufferedPort<iCub::skinDynLib::skinContactList> * port_skin_contacts;

        yarp::sig::Vector           q, qStamps;         // last joint position estimation
        yarp::sig::Vector           tauJ, tauJStamps;

        std::vector<yarp::sig::Vector> forcetorques;
        yarp::sig::Vector forcetorquesStamps;

        std::vector<yarp::sig::Vector> IMUs;
        yarp::sig::Vector IMUStamps;

        //Data structures related to skin
        iCub::skinDynLib::skinContactList skinContacts;
        double skin_contact_listStamp;
        double last_reading_skin_contact_list_Stamp;

        iCub::skinDynLib::dynContactList dynContacts;

        //Estimation options
        bool enable_omega_domega_IMU;
        int min_taxel;
        bool assume_fixed_base;
        std::string fixed_link;
        bool assume_fixed_base_from_odometry;
        yarp::os::Property wbi_yarp_conf;

        yarp::sig::Vector omega_used_IMU;
        yarp::sig::Vector domega_used_IMU;
        yarp::sig::Vector ddp_used_IMU;

        /* Resize all vectors using current number of DoFs. */
        void resizeAll(int n);

        //< \todo TODO add general interface using type (?) of sensors

        /* Resize all FT sensors related vectors using current number of Force Torque sensors */
        void resizeFTs(int n);

        /* Resize all IMU sensors related vectors using current number of IMU sensors */
        void resizeIMUs(int n);

        /** Read the skin contacts and generated the contact points for external wrenches  estimation */
        void readSkinContacts();

        /** For a given subtree, get the default contact point (the one used if there are now contacts coming from the skin */
        iCub::skinDynLib::dynContact getDefaultContact(const int subtree_id);
        std::vector<TorqueEstimationSubtree> torque_estimation_subtrees;
        std::vector<int>                     contacts_for_given_subtree;
        std::vector<int>                     link2subtree;


        /**  Estimate internal torques and external forces from measured sensors, using iDynTree library */
        void estimateExternalForcesAndJointTorques(RobotJointStatus & joint_status, RobotSensorStatus & sensor_status);

        /** Store external wrenches ad the end effectors */
        void readEndEffectorsExternalWrench();

    public:
        iCub::iDynTree::TorqueEstimationTree * robot_estimation_model;
        iCub::iDynTree::TorqueEstimationTree * robot_estimation_model_on_l_sole;
        iCub::iDynTree::TorqueEstimationTree * robot_estimation_model_on_r_sole;

        std::string current_fixed_link_name;


        iCub::skinDynLib::dynContactList estimatedLastDynContacts;
        iCub::skinDynLib::skinContactList estimatedLastSkinDynContacts;

         /** Constructor.
         *
         * @param port_skin_contacts pointer to a port reading a skinContactList from the robot skin
         * \todo TODO skin_contacts should be read from the WholeBodySensors interface
         */
        ExternalWrenchesAndTorquesEstimator(int _period,
                                       yarpWbi::yarpWholeBodySensors *_sensors,
                                       yarp::os::BufferedPort<iCub::skinDynLib::skinContactList> * _port_skin_contacts,
                                       yarp::os::Property & _wbi_yarp_conf);

        bool lockAndSetEstimationParameter(const wbi::EstimateType et, const wbi::EstimationParameter ep, const void *value);

        bool init();
        void estimateExternalWrenchAndInternalJoints(RobotJointStatus & joint_status, RobotSensorStatus & sensor_status);
        void fini();

        bool setEnableOmegaDomegaIMU(bool opt);
        /** Set the minimum number of activated taxels an skin contact should have to be considered by the estimation  */
        bool setMinTaxel(const int min_taxel);


    };

#endif
