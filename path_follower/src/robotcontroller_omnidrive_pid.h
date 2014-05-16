#ifndef ROBOTCONTROLLER_OMNIDRIVE_PID_H
#define ROBOTCONTROLLER_OMNIDRIVE_PID_H

/// THIRD PARTY
#include <Eigen/Core>

/// PROJECT
#include "robotcontroller.h"
#include "PidCtrl.h"
#include "visualizer.h"

class RobotController_Omnidrive_Pid : public RobotController
{
public:
    RobotController_Omnidrive_Pid(ros::Publisher &cmd_publisher, BehaviouralPathDriver *path_driver);

    virtual void publishCommand();

    //! Immediatley stop any motion.
    virtual void stopMotion();

    virtual void initOnLine();


protected:
    virtual void behaveOnLine();

    virtual void behaveAvoidObstacle();

    //! Return true, when turning point is reached.
    virtual bool behaveApproachTurningPoint();


private:
    struct Command
    {
        // was wird hier gebraucht? Wahrscheinlich 2d-vektor für velocity und scalar für rotation?!
        // -> jupp, vektor aber besser in polarkoordinaten

        //! Speed of the movement.
        float speed;
        //! Direction of movement as angle to the current robot orientation.
        float direction_angle;
        //! rotational velocity.
        float rotation;


        // initialize all values to zero
        Command():
            speed(0.0f), direction_angle(0.0f), rotation(0.0f)
        {}

        operator geometry_msgs::Twist()
        {
            geometry_msgs::Twist msg;
            msg.linear.x  = cos(direction_angle);
            msg.linear.y  = sin(direction_angle);
            msg.angular.z = rotation;
            return msg;
        }

        bool isValid()
        {
            if ( isnan(speed) || isinf(speed)
                 || isnan(direction_angle) || isinf(direction_angle)
                 || isnan(rotation) || isinf(rotation) )
            {
                ROS_FATAL("Non-numerical values in command: %d,%d,%d,%d,%d,%d",
                          isnan(speed), isinf(speed),
                          isnan(direction_angle), isinf(direction_angle),
                          isnan(rotation), isinf(rotation));
                // fix this instantly, to avoid further problems.
                speed = 0.0;
                direction_angle = 0.0;
                rotation = 0.0;

                return false;
            } else {
                return true;
            }
        }
    };

    struct ControllerOptions
    {
        double dead_time_;
    };

    Visualizer *visualizer_;
    PidCtrl pid_direction_;
    PidCtrl pid_rotation_;
    Command cmd_;
    ControllerOptions options_;

    void configure();

    bool setCommand(double e_distance, double e_rotation);

    Eigen::Vector2d predictPosition();
    double calculateLineError();
};

#endif // ROBOTCONTROLLER_OMNIDRIVE_PID_H
