#ifndef WORKERS_H
#define WORKERS_H

#include <type_traits>
#include <string>
#include <memory>
#include <functional>
#include <iostream>

#include "internal/template_utils.h"
#include "internal/enum_misc.h"

namespace work
{
  enum class action_t
  {
    save,
    load
  };

  struct has_free_func {};
  struct has_member_func {};
  struct is_base_for_worker {};
  struct has_no_func {};

  template<typename K, typename V>
  struct kv_element
  {
    K k;
    std::reference_wrapper<V> v;

    kv_element (K key, V &value_ref)
      : k (key),
        v (value_ref) {}
  };

  template<typename T>
  using vector_element = kv_element<size_t, T>;

  namespace detail
  {
    template<typename T, typename Worker, typename = decltype (work::declref<Worker> ().process_base (work::declref<T> ()))>
    is_base_for_worker tag_creator_impl (work::priority_tag_1) {return is_base_for_worker ();}

    template<typename T, typename Worker, typename = decltype (worker_process (work::declref<Worker> (), work::declref<T> ()))>
    has_free_func      tag_creator_impl (work::priority_tag_2) {return has_free_func ();}

    template<typename T, typename Worker, typename = decltype (work::declref<T> ().worker_process (work::declref<Worker> ()))>
    has_member_func    tag_creator_impl (work::priority_tag_2) {return has_member_func ();}

    template<typename T, typename Worker>
    has_no_func        tag_creator_impl (work::priority_tag_3) {return has_no_func ();}

  }

  template<typename T, typename Worker>
  struct tag_creator
  {
    using tag = decltype (detail::tag_creator_impl<T, Worker> (work::priority_tag ()));
  };

  template<typename T, typename Worker>
  bool process (Worker &worker, T &obj, const std::string &name)
  {
    worker.set_next_name (name);
    bool success = process_by_tag (typename tag_creator<T, Worker>::tag (), worker, obj);
    worker.back ();
    return success;
  }

  template<typename T, typename Worker, typename = work::use_if_primitive<T>>
  bool worker_process (Worker &worker, T &obj)
  {
    return worker.process_base (obj);
  }

  template<typename K, typename V, typename Worker>
  bool worker_process (Worker &worker, kv_element<K,V> &el)
  {
    work::process (worker, el.k, "k");
    work::process (worker, el.v.get (), "v");
    return worker.is_ok ();
  }

  template<typename T, typename Worker>
  bool process_by_tag (has_free_func, Worker &worker, T &obj)
  {
    return worker_process (worker, obj);
  }

  template<typename T, typename Worker>
  bool process_by_tag (has_member_func, Worker &worker, T &obj)
  {
    return obj.worker_process (worker);
  }

  template<typename T, typename Worker>
  bool process_by_tag (is_base_for_worker, Worker &worker, T &obj)
  {
    return worker.process_base (obj);
  }

  template<typename T, typename Worker>
  bool process_by_tag (has_no_func, Worker &, T &)
  {
    static_assert (std::is_void_v<T>, "No processing function is defined for T");
    return false;
  }
}

#endif // WORKERS_H
