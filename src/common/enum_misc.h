#ifndef ENUM_MISC_H
#define ENUM_MISC_H

#include "enum_range.h"

#include "common/debug_utils.h"

template<typename Enum, typename = use_if_enum<Enum>, typename = decltype (enum_to_string (Enum ()))>
Enum string_to_enum (std::string string)
{
  for (Enum e : enum_range<Enum> ())
    {
      if (!strcmp (enum_to_string (e).c_str (), string.c_str ()))
        return e;
    }
  DEBUG_PAUSE ("Failed to find");
  return enum_end (Enum ()); //implement enum_end (Enum) or Enum::COUNT;
}

template<typename Enum, typename = use_if_enum<Enum>>
int enum_size (Enum) {return static_cast<int> (enum_end (Enum ())) - static_cast<int> (enum_begin (Enum ()));}

template<typename T, typename Enum, typename = typename std::enable_if<std::is_integral<T>::value, T>::type, typename = use_if_enum<Enum>>
T enum_cast (Enum e) {return static_cast<T> (e) - static_cast<T> (enum_begin (Enum ()));}

template<typename Enum, typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type, typename = use_if_enum<Enum>>
Enum enum_cast (const T t, ...) {return static_cast<Enum> (t + static_cast<T> (enum_begin (Enum ())));}

template<typename Enum, typename = use_if_enum<Enum>>
std::vector<Enum> vector_of (Enum)
{
  std::vector<Enum> retval;
  for (Enum e : enum_range<Enum> ())
    retval.push_back (e);

  return retval;
}

#endif // ENUM_MISC_H
