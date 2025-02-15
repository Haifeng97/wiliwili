

// Switch include only necessary for demo videos recording
#ifdef __SWITCH__
#include <switch.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <borealis.hpp>
#include <string>

#include "utils/config_helper.hpp"
#include "activity/main_activity.hpp"
#include "activity/search_activity.hpp"
//#include "activity/splash_activity.hpp"
#include "activity/hint_activity.hpp"
#include "activity/player_activity.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <unistd.h>

#define DISK_LOG

using namespace brls::literals; // for _i18n

int main(int argc, char* argv[])
{

#ifdef __SWITCH__
//    appletInitializeGamePlayRecording();
#endif
    // Set min_threads and max_threads of http thread pool
    curl_global_init(CURL_GLOBAL_DEFAULT);
    cpr::async::startup(1, THREAD_POOL_MAX_THREAD_NUM);

    // Set log level
    brls::Logger::setLogLevel(brls::LogLevel::ERROR);
    brls::Logger::debug("std::thread::hardware_concurrency(): {}", std::thread::hardware_concurrency());

#ifdef DISK_LOG
    std::filesystem::create_directories(ConfigHelper::getConfigDir());
    std::ofstream logFile(ConfigHelper::getConfigDir() + "/log.txt");
    brls::Logger::getLogEvent()->subscribe([&logFile](std::string log) {
        logFile << log << std::endl;
    });
#endif

    // Init the app and i18n
    if (!brls::Application::init())
    {
        brls::Logger::error("Unable to init Borealis application");
        return EXIT_FAILURE;
    }
    brls::Application::createWindow("wiliwili/title"_i18n);

    // Have the application register an action on every activity that will quit when you press BUTTON_START
    brls::Application::setGlobalQuit(false);

    brls::Logger::info("createWindow done");
    // Load Cookies for bilibili from disk
    ConfigHelper::init();

    // Register custom view\theme\style
    Register::initCustomView();
    Register::initCustomTheme();
    Register::initCustomStyle();

    if(brls::Application::getPlatform()->isApplicationMode()){
        brls::Application::pushActivity(new MainActivity());
    } else {
        brls::Application::pushActivity(new HintActivity());
    }
//    brls::Application::pushActivity(new SplashActivity());
//    brls::Application::pushActivity(new PlayerActivity("BV1A44y1u7PF"));

    // Run the app
    while (brls::Application::mainLoop()){

    }

    brls::Logger::debug("main loop done");
    cpr::async::cleanup();
    curl_global_cleanup();

#ifdef DISK_LOG
    logFile.close();
#endif
    // Exit
    return EXIT_SUCCESS;
}
