#include <gz/gui/Plugin.hh>
#include <gz/gui/Application.hh>
#include <gz/gui/MainWindow.hh>
#include <gz/gui/GuiEvents.hh>

#include <gz/msgs/pose.pb.h>
#include <gz/transport/Node.hh>

#include <gz/math/Vector3.hh>

#include <gz/plugin/Register.hh>

#include <QEvent>
#include <QObject>

#include <iostream>

namespace br_gazebo_click_goal
{

class BrClickGoalPlugin : public gz::gui::Plugin
{
public:
    BrClickGoalPlugin()
    {
        std::cout
            << "\n========================================\n"
            << "[BR CLICK GOAL] Plugin created\n"
            << "========================================\n"
            << std::endl;

        this->pub_ =
            this->node_.Advertise<gz::msgs::Pose>("/br/click_goal");

        if (!this->pub_)
        {
            std::cerr
                << "[BR CLICK GOAL] ERROR: Cannot advertise "
                << "/br/click_goal"
                << std::endl;
        }
        else
        {
            std::cout
                << "[BR CLICK GOAL] Publisher ready: "
                << "/br/click_goal"
                << std::endl;
        }
    }

    ~BrClickGoalPlugin() override
    {
        std::cout
            << "[BR CLICK GOAL] Plugin destroyed"
            << std::endl;
    }

protected:
    void LoadConfig(
        const tinyxml2::XMLElement *_config) override
    {
        gz::gui::Plugin::LoadConfig(_config);

        auto *app = gz::gui::App();

        if (app == nullptr)
        {
            std::cerr
                << "[BR CLICK GOAL] ERROR: Gazebo GUI App not found"
                << std::endl;
            return;
        }

        auto *mainWindow =
            app->findChild<gz::gui::MainWindow *>();

        if (mainWindow == nullptr)
        {
            std::cerr
                << "[BR CLICK GOAL] ERROR: MainWindow not found"
                << std::endl;
            return;
        }

        mainWindow->installEventFilter(this);

        std::cout
            << "[BR CLICK GOAL] Mouse event filter installed"
            << std::endl;
    }

    bool eventFilter(
        QObject *_obj,
        QEvent *_event) override
    {
        if (_event == nullptr)
        {
            return QObject::eventFilter(_obj, _event);
        }

        if (_event->type() ==
            gz::gui::events::LeftClickToScene::kType)
        {
            auto *clickEvent =
                dynamic_cast<
                    gz::gui::events::LeftClickToScene *
                >(_event);

            if (clickEvent != nullptr)
            {
                const gz::math::Vector3d point =
                    clickEvent->Point();

                const double x = point.X();
                const double y = point.Y();
                const double z = point.Z();

                std::cout
                    << "\n"
                    << "========================================\n"
                    << "[BR CLICK GOAL] CLICK DETECTED\n"
                    << "X = " << x << " m\n"
                    << "Y = " << y << " m\n"
                    << "Z = " << z << " m\n"
                    << "========================================\n"
                    << std::endl;

                gz::msgs::Pose goal;

                goal.mutable_position()->set_x(x);
                goal.mutable_position()->set_y(y);
                goal.mutable_position()->set_z(z);

                // Orientation mặc định: yaw = 0
                goal.mutable_orientation()->set_x(0.0);
                goal.mutable_orientation()->set_y(0.0);
                goal.mutable_orientation()->set_z(0.0);
                goal.mutable_orientation()->set_w(1.0);

                if (this->pub_)
                {
                    this->pub_.Publish(goal);

                    std::cout
                        << "[BR CLICK GOAL] Published: "
                        << "/br/click_goal"
                        << std::endl;
                }
            }
        }

        return QObject::eventFilter(_obj, _event);
    }

private:
    gz::transport::Node node_;

    gz::transport::Node::Publisher pub_;
};

}  // namespace br_gazebo_click_goal

GZ_ADD_PLUGIN(
    br_gazebo_click_goal::BrClickGoalPlugin,
    gz::gui::Plugin
)
