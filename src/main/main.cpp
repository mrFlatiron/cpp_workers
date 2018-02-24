#include "gtest/gtest.h"
#include <cstdio>


#include "work/workers.h"
#include "work/xml_worker.h"
#include "work/std_containers_impls.h"

enum class some_enum
{
  one,
  two,
  COUNT
};

std::string enum_to_string (some_enum e)
{
  switch (e)
    {
    case some_enum::one:
      return "one";
    case some_enum::two:
      return "I love Darkin";
    case some_enum::COUNT:
      return "";
    }

  return "";
}

struct has_free
{
  int i;
  int j;
  std::vector<int> vec;
  std::string color;
  some_enum en;
  std::map<some_enum, std::vector<int>> map;
};

bool worker_process (work::tab_worker &worker, has_free &obj)
{
  work::process (worker, obj.i, "i_member");
  work::process (worker, obj.j, "j_member");
//  work::process (worker, obj.vec, "vec_me");
  work::process (worker, obj.color, "color");
  work::process (worker, obj.en, "en");
  return true;
}

bool worker_process (work::xml_worker &worker, has_free &obj)
{
  work::process (worker, obj.i, "i_member");
  work::process (worker, obj.j, "j_member");
  work::process (worker, obj.vec, "vec_me");
  work::process (worker, obj.color, "color");
  work::process (worker, obj.en, "en");
  work::process (worker, obj.map, "map");
  return true;
}

int main (int argc, char *argv[])
{
//  testing::InitGoogleTest (&argc, argv);

//  if (!RUN_ALL_TESTS ())
//    fprintf (stdout, "Success!\n");
//  else
//    fprintf (stdout, "Failure!\n");

  (void)argc;
  (void)argv;

  has_free d;
//  d.i = 5;
//  d.j = 10;
//  d.vec.resize (7, 1);
//  d.color = "red";
//  d.en = some_enum::two;
//  std::vector one ({1, 2, 3});
//  std::vector two ({4, 5, 6});
//  d.map.emplace (some_enum::one, std::move (one));
//  d.map.emplace (some_enum::two, std::move (two));

  work::xml_worker worker (work::action_t::load, "savetest.xml");

  work::process (worker, d, "has_free_struct");

  return 0;
}
