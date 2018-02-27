#ifndef STD_CONTAINERS_IMPLS_H
#define STD_CONTAINERS_IMPLS_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <map>

#include "workers.h"

namespace work
{
  template<typename T, typename Worker>
  bool worker_process (Worker &worker, std::vector<T> &vec)
  {
    size_t size = vec.size ();
    work::process (worker, size, "size");


    if (worker.action () == action_t::load)
      vec.resize (size);

    for (size_t i = 0; i < size; i++)
      {
        vector_element<T> el (i, vec[i]);
        work::process (worker, el, "el");
      }

    return worker.is_ok ();
  }

  template<typename Worker, typename... Mapargs>
  bool worker_process (Worker &worker, std::unordered_map<Mapargs...> &umap)
  {
    size_t size = umap.size ();
    work::process (worker, size, "size");

    if (worker.action () == action_t::save)
      for (auto &p : umap)
        {
          work::kv_element temp_pair (p.first, p.second);
          work::process (worker, temp_pair, "el");
        }

    if (worker.action () == action_t::load)
      {
        typename std::unordered_map<Mapargs...>::value_type temp_p;
        for (size_t i = 0; i < size; i++)
          {
            work::kv_element temp_kv_el (temp_p.first, temp_p.second);

            work::process (worker, temp_kv_el, "el");
            umap.emplace (std::move (temp_kv_el.k), std::move (temp_kv_el.v.get ()));
          }
      }

    return worker.is_ok ();
  }

  template<typename Worker, typename... Mapargs>
  bool worker_process (Worker &worker, std::map<Mapargs...> &map)
  {
    size_t size = map.size ();
    work::process (worker, size, "size");

    if (worker.action () == action_t::save)
      for (auto &p : map)
        {
          work::kv_element temp_pair (p.first, p.second);
          work::process (worker, temp_pair, "el");
        }

    if (worker.action () == action_t::load)
      {
        typename std::map<Mapargs...>::value_type temp_p;
        for (size_t i = 0; i < size; i++)
          {
            work::kv_element temp_kv_el (temp_p.first, temp_p.second);

            work::process (worker, temp_kv_el, "el");
            map.emplace (std::move (temp_kv_el.k), std::move (temp_kv_el.v.get ()));
          }
      }

    return worker.is_ok ();
  }

  template<typename K, typename V, typename Worker>
  bool worker_process (Worker &worker, std::pair<K, V> &pair)
  {
    work::process (worker, pair.first, "first");
    work::process (worker, pair.second, "second");
    return worker.is_ok ();
  }

  template<typename T, typename Worker>
  bool worker_process (Worker &worker, std::unique_ptr<T> &ptr)
  {
      if (worker.action () == action_t::load)
        ptr = std::make_unique<T> ();

      return work::process (worker, *ptr, "uptr");
  }
}

#endif // STD_CONTAINERS_IMPLS_H
