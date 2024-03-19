#include "pch.h"

#include "AGT/error/CrashHandler.h"

TEST(CrashHandler, Init) {
    AGT::CrashHandler::Init(AGT::MinidumpSize::Small, "coredump.dmp", []() {});
}