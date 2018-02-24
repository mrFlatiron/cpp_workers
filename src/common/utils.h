#ifndef UTILS_H
#define UTILS_H

#include <type_traits>

#include <string>
#include <cstdlib>

template<typename T>
int isize (const T &container) {return static_cast<int> (container.size ());}

template<typename T, typename Iter = typename T::const_iterator>
Iter u_last (const T &container)
{
  if (!container.size ())
    return container.end ();
  auto it = container.end ();
  it--;
  return it;
}

//#if __cplusplus < 201402L
template<typename T, typename... Args>
std::unique_ptr<T> make_unique (Args&&... args)
{
  return std::unique_ptr<T> (new T (std::forward<Args>(args)...));
}

//#endif // __cplusplus

template<typename T, typename... Args>
void put_in (std::unique_ptr<T> &something, Args&&... args)
{
  something = make_unique<T> (std::forward<Args> (args)...);
}

template<typename K, typename V, class C>
V find_default_kv (const C &container, const K &to_find, V &&default_v)
{
  auto it = container.find (to_find);
  if (it == container.end ())
    return std::forward<V> (default_v);

  return it->second;
}

template<typename K, class C>
K find_default_k (const C &container, const K &to_find, K &&default_k)
{
  auto it = container.find (to_find);
  if (it == container.end ())
    return std::forward<K> (default_k);

  return *it;
}

template<typename T>
T from_string (std::string_view str)
{
  if constexpr (std::is_same_v<T, double>)
  {
    return strtod (str.data (), nullptr);
  }
  if constexpr (std::is_same_v<T, float>)
  {
    return strtof (str.data (), nullptr);
  }
  if constexpr (std::is_same_v<T, long double>)
  {
    return strtold (str.data (), nullptr);
  }
  if constexpr (std::is_same_v<T, int>)
  {
    return strtol (str.data (), nullptr, 10);
  }
  if constexpr (std::is_same_v<T, long>)
  {
    return strtol (str.data (), nullptr, 10);
  }
  if constexpr (std::is_same_v<T, unsigned long>)
  {
    return strtoul (str.data (), nullptr, 10);
  }
  if constexpr (std::is_same_v<T, long long>)
  {
    return strtoll (str.data (), nullptr, 10);
  }
  if constexpr (std::is_same_v<T, unsigned long long>)
  {
    return strtoull (str.data (), nullptr, 10);
  }
}

#endif // UTILS_H
