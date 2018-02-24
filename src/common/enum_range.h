#ifndef ENUM_RANGE_H
#define ENUM_RANGE_H

#include "enum_helper.h"

template<typename Enum, typename = use_if_enum<Enum>>
class enum_range
{
public:
  enum_range () {}

  enum_helper<Enum> begin () { return enum_helper<Enum> (enum_begin (Enum ())); }
  enum_helper<Enum> end   () { return enum_helper<Enum> (enum_end (Enum ()));   } //implement enum_end (Enum) or Enum::COUNT;
};

#endif // ENUM_RANGE_H
