//
// Copyright (C) 2012 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// based on pam_google_testrunner.cc

#include <string>

#include <xz.h>

#include <base/at_exit.h>
#include <base/command_line.h>
#include <base/environment.h>
#include <brillo/test_helpers.h>
#include <gtest/gtest.h>

#include "update_engine/common/terminator.h"
#include "update_engine/common/test_utils.h"
#include "update_engine/payload_generator/xz.h"

int main(int argc, char **argv) {
  LOG(INFO) << "started";
  base::AtExitManager exit_manager;
  // xz-embedded requires to initialize its CRC-32 table once on startup.
  xz_crc32_init();
  // The LZMA SDK-based Xz compressor used in the payload generation requires
  // this one-time initialization.
  chromeos_update_engine::XzCompressInit();
  // TODO(garnold) temporarily cause the unittest binary to exit with status
  // code 2 upon catching a SIGTERM. This will help diagnose why the unittest
  // binary is perceived as failing by the buildbot.  We should revert it to use
  // the default exit status of 1.  Corresponding reverts are necessary in
  // terminator_unittest.cc.
  chromeos_update_engine::Terminator::Init(2);
  // In Android bsdiff is located in update_engine_unittests, add it to PATH.
#ifdef __ANDROID__
  std::unique_ptr<base::Environment> env(base::Environment::Create());
  std::string path_env;
  CHECK(env->GetVar("PATH", &path_env));
  path_env +=
      ":" + chromeos_update_engine::test_utils::GetBuildArtifactsPath().value();
  CHECK(env->SetVar("PATH", path_env));
#endif
  LOG(INFO) << "parsing command line arguments";
  base::CommandLine::Init(argc, argv);
  LOG(INFO) << "initializing gtest";
  SetUpTests(&argc, argv, true);
  LOG(INFO) << "running unit tests";
  int test_result = RUN_ALL_TESTS();
  LOG(INFO) << "unittest return value: " << test_result;
  return test_result;
}
