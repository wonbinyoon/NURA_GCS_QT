#include <gtest/gtest.h>
#include "ui/AppController.h"

TEST(AppControllerTest, BasicTest) {
    AppController controller;
    EXPECT_EQ(controller.getMode(), AppController::Mode::LIVE);
    controller.setMode(AppController::Mode::REPLAY);
    EXPECT_EQ(controller.getMode(), AppController::Mode::REPLAY);
}
