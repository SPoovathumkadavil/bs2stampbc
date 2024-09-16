
#include <gtest/gtest.h>
#include <filesystem>

#include <project.hpp>

TEST(LibTests, FileExistsTest)
{
  ASSERT_TRUE(std::filesystem::exists("resources/test.bs2"));
}

TEST(ProjectTest, CompileTestFile)
{
  Project proj;
  proj.setSourceFile(0, "resources/test.bs2");
  proj.setObjectFile(0, "resources/test.obj");
  proj.setFlags(Project::F_COMPILEONLY
            | Project::F_IGNPROJECT);
  int ret = proj.process();
  ASSERT_TRUE(ret > 0);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

