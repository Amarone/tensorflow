/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "tensorflow/lite/testing/tflite_driver.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace tflite {
namespace testing {
namespace {

using ::testing::ElementsAre;

TEST(TfliteDriverTest, SimpleTest) {
  std::unique_ptr<TestRunner> runner(new TfLiteDriver());

  runner->SetModelBaseDir("tensorflow/lite");
  runner->LoadModel("testdata/multi_add.bin");
  ASSERT_TRUE(runner->IsValid());

  ASSERT_THAT(runner->GetInputs(), ElementsAre(0, 1, 2, 3));
  ASSERT_THAT(runner->GetOutputs(), ElementsAre(5, 6));

  for (int i : {0, 1, 2, 3}) {
    runner->ReshapeTensor(i, "1,2,2,1");
  }
  ASSERT_TRUE(runner->IsValid());

  runner->AllocateTensors();

  runner->SetInput(0, "0.1,0.2,0.3,0.4");
  runner->SetInput(1, "0.001,0.002,0.003,0.004");
  runner->SetInput(2, "0.001,0.002,0.003,0.004");
  runner->SetInput(3, "0.01,0.02,0.03,0.04");

  runner->ResetTensor(2);

  runner->SetExpectation(5, "0.101,0.202,0.303,0.404");
  runner->SetExpectation(6, "0.011,0.022,0.033,0.044");

  runner->Invoke();
  ASSERT_TRUE(runner->IsValid());

  ASSERT_TRUE(runner->CheckResults());
  EXPECT_EQ(runner->ReadOutput(5), "0.101,0.202,0.303,0.404");
  EXPECT_EQ(runner->ReadOutput(6), "0.011,0.022,0.033,0.044");
}

TEST(TfliteDriverTest, SimpleTestWithSignature) {
  std::unique_ptr<TestRunner> runner(new TfLiteDriver);

  runner->SetModelBaseDir("tensorflow/lite");
  runner->LoadModel("testdata/multi_add_signature.bin", "serving_default");
  ASSERT_TRUE(runner->IsValid());

  for (const auto& i : {"a", "b", "c", "d"}) {
    runner->ReshapeTensor(i, "1,2,2,1");
  }
  ASSERT_TRUE(runner->IsValid());

  runner->AllocateTensors();

  runner->ResetTensor("c");
  runner->Invoke({{"a", "0.1,0.2,0.3,0.4"},
                  {"b", "0.001,0.002,0.003,0.004"},
                  {"d", "0.01,0.02,0.03,0.04"}});

  ASSERT_TRUE(runner->IsValid());

  ASSERT_EQ(runner->ReadOutput("x"), "0.101,0.202,0.303,0.404");
  ASSERT_EQ(runner->ReadOutput("y"), "0.011,0.022,0.033,0.044");

  ASSERT_TRUE(runner->CheckResults(
      {{"x", "0.101,0.202,0.303,0.404"}, {"y", "0.011,0.022,0.033,0.044"}},
      /*expected_output_shapes=*/{}));
}

TEST(TfliteDriverTest, SingleAddOpTest) {
  std::unique_ptr<TestRunner> runner(new TfLiteDriver(
      /*delegate_type=*/TfLiteDriver::DelegateType::kNone,
      /*reference_kernel=*/true));

  runner->SetModelBaseDir("tensorflow/lite");
  runner->LoadModel("testdata/multi_add.bin");
  ASSERT_TRUE(runner->IsValid());

  ASSERT_THAT(runner->GetInputs(), ElementsAre(0, 1, 2, 3));
  ASSERT_THAT(runner->GetOutputs(), ElementsAre(5, 6));

  for (int i : {0, 1, 2, 3}) {
    runner->ReshapeTensor(i, "1,2,2,1");
  }
  ASSERT_TRUE(runner->IsValid());

  runner->AllocateTensors();

  runner->SetInput(0, "0.1,0.2,0.3,0.4");
  runner->SetInput(1, "0.001,0.002,0.003,0.004");
  runner->SetInput(2, "0.001,0.002,0.003,0.004");
  runner->SetInput(3, "0.01,0.02,0.03,0.04");

  runner->ResetTensor(2);

  runner->SetExpectation(5, "0.101,0.202,0.303,0.404");
  runner->SetExpectation(6, "0.011,0.022,0.033,0.044");

  runner->Invoke();
  ASSERT_TRUE(runner->IsValid());

  ASSERT_TRUE(runner->CheckResults());
  EXPECT_EQ(runner->ReadOutput(5), "0.101,0.202,0.303,0.404");
  EXPECT_EQ(runner->ReadOutput(6), "0.011,0.022,0.033,0.044");
}

TEST(TfliteDriverTest, AddOpWithNaNTest) {
  std::unique_ptr<TestRunner> runner(new TfLiteDriver(
      /*delegate_type=*/TfLiteDriver::DelegateType::kNone,
      /*reference_kernel=*/true));

  runner->SetModelBaseDir("tensorflow/lite");
  runner->LoadModel("testdata/multi_add.bin");
  ASSERT_TRUE(runner->IsValid());

  ASSERT_THAT(runner->GetInputs(), ElementsAre(0, 1, 2, 3));
  ASSERT_THAT(runner->GetOutputs(), ElementsAre(5, 6));

  for (int i : {0, 1, 2, 3}) {
    runner->ReshapeTensor(i, "1,2,2,1");
  }
  ASSERT_TRUE(runner->IsValid());

  runner->AllocateTensors();

  runner->SetInput(0, "0.1,nan,0.3,0.4");
  runner->SetInput(1, "0.001,0.002,0.003,0.004");
  runner->SetInput(2, "0.001,0.002,0.003,0.004");
  runner->SetInput(3, "0.01,0.02,0.03,nan");

  runner->ResetTensor(2);

  runner->SetExpectation(5, "0.101,nan,0.303,0.404");
  runner->SetExpectation(6, "0.011,0.022,0.033,nan");

  runner->Invoke();
  ASSERT_TRUE(runner->IsValid());

  ASSERT_TRUE(runner->CheckResults());
  EXPECT_EQ(runner->ReadOutput(5), "0.101,nan,0.303,0.404");
  EXPECT_EQ(runner->ReadOutput(6), "0.011,0.022,0.033,nan");
}

TEST(TfliteDriverTest, AddQuantizedInt8Test) {
  std::unique_ptr<TestRunner> runner(new TfLiteDriver());

  runner->SetModelBaseDir("tensorflow/lite");
  runner->LoadModel("testdata/add_quantized_int8.bin");
  ASSERT_TRUE(runner->IsValid());

  ASSERT_THAT(runner->GetInputs(), ElementsAre(1));
  ASSERT_THAT(runner->GetOutputs(), ElementsAre(2));

  runner->ReshapeTensor(1, "1,2,2,1");
  ASSERT_TRUE(runner->IsValid());

  runner->AllocateTensors();

  runner->SetInput(1, "1,1,1,1");

  runner->SetExpectation(2, "0.0117,0.0117,0.0117,0.0117");

  runner->Invoke();
  ASSERT_TRUE(runner->IsValid());

  ASSERT_TRUE(runner->CheckResults());
  EXPECT_EQ(runner->ReadOutput(2), "3,3,3,3");
}

}  // namespace
}  // namespace testing
}  // namespace tflite
