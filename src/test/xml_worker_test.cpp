#include "gtest/gtest.h"

#include "work/work.h"

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
      return "two";
    case some_enum::COUNT:
      return "";
    }

  return "";
}

struct has_free
{
  int i;
  double d;
  std::vector<int> vec;
  std::string color;
  some_enum en;
  std::map<some_enum, std::unique_ptr<std::vector<int>>> map;

};

struct has_member
{
  std::unique_ptr<std::unordered_map<some_enum, std::unique_ptr<std::string>>> uptr;
  bool worker_process (work::xml_worker &worker)
  {
    return work::process (worker, uptr, "uptr_member");
  }
};

bool worker_process (work::xml_worker &worker, has_free &obj)
{
  work::process (worker, obj.i, "i_member");
  work::process (worker, obj.d, "d_member");
  work::process (worker, obj.vec, "vec_me");
  work::process (worker, obj.color, "color");
  work::process (worker, obj.en, "en");
  work::process (worker, obj.map, "map");
  return worker.is_ok ();
}

TEST (xmlWorker, all)
{
  has_free free_obj;

  std::vector vec_1 {6, 7, 8};
  std::vector vec_2 {9, 10, 11};

  free_obj.i = 5;
  free_obj.d = 25 + 213e-10;
  free_obj.vec = std::vector ({4, 5, 6});
  free_obj.color = "blue";
  free_obj.en = some_enum::two;
  free_obj.map.emplace (some_enum::one, std::make_unique<std::vector<int>> (vec_1));
  free_obj.map.emplace (some_enum::two, std::make_unique<std::vector<int>> (vec_2));

  has_member member_obj;
  work::put_in (member_obj.uptr);
  member_obj.uptr->emplace (some_enum::two, std::make_unique<std::string> ("</value>Nasty string"));

{
  work::xml_worker worker (work::action_t::save, "test_save.xml");

  work::process (worker, free_obj, "free_obj");
  work::process (worker, member_obj, "member_obj");

  EXPECT_TRUE (worker.is_ok ());
 }

  free_obj.i = 0;
  free_obj.d = 22;
  free_obj.vec = std::vector ({6});
  free_obj.color = "red";
  free_obj.en = some_enum::one;
  free_obj.map.emplace (some_enum::two, std::make_unique<std::vector<int>> (1, -6));
  free_obj.map.emplace (some_enum::one, std::make_unique<std::vector<int>> (1, -9));

  member_obj.uptr.reset ();

  {
    work::xml_worker worker (work::action_t::load, "test_save.xml");

    work::process (worker, free_obj, "free_obj");
    work::process (worker, member_obj, "member_obj");

    EXPECT_TRUE (worker.is_ok ());
   }

  EXPECT_EQ (free_obj.i, 5);
  EXPECT_EQ (free_obj.d, 25 + 213e-10);
  EXPECT_EQ (free_obj.color, "blue");
  EXPECT_EQ (free_obj.en, some_enum::two);
  EXPECT_EQ (static_cast<int> (free_obj.vec.size ()), 3);
  EXPECT_EQ (free_obj.vec[0], 4);
  EXPECT_EQ (free_obj.vec[1], 5);
  EXPECT_EQ (free_obj.vec[2], 6);
  auto it = free_obj.map.find (some_enum::one);
  EXPECT_TRUE (it != free_obj.map.end ());
  EXPECT_EQ (static_cast<int> (it->second->size ()), 3);
  EXPECT_EQ (it->second->operator[] (0), 6);
  EXPECT_EQ (it->second->operator[] (1), 7);
  EXPECT_EQ (it->second->operator[] (2), 8);
  it = free_obj.map.find (some_enum::two);
  EXPECT_TRUE (it != free_obj.map.end ());
  EXPECT_EQ (static_cast<int> (it->second->size ()), 3);
  EXPECT_EQ (it->second->operator[] (0), 9);
  EXPECT_EQ (it->second->operator[] (1), 10);
  EXPECT_EQ (it->second->operator[] (2), 11);
  EXPECT_TRUE (member_obj.uptr);
  auto it_2 = member_obj.uptr->find (some_enum::two);
  EXPECT_TRUE (it_2 != member_obj.uptr->end ());
  EXPECT_TRUE (it_2->second);
  EXPECT_EQ (*(it_2->second), "</value>Nasty string");
}
