#include "gtest/gtest.h"

int main (int argc, char *argv[])
{
  testing::InitGoogleTest (&argc, argv);

  if (!RUN_ALL_TESTS ())
    fprintf (stdout, "Success!\n");
  else
    fprintf (stdout, "Failure!\n");

  return 0;
}
