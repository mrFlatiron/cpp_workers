#ifndef TEMPLATE_UTILS_H
#define TEMPLATE_UTILS_H

#include <type_traits>
#include <utility>

namespace templ
{
  template<typename Func, typename Tuple, size_t... I>
  auto try_call (Func &&f, Tuple && t, std::index_sequence<I...>) -> decltype (f (std::get<I> (t)...));

  template<typename Func, typename Tuple, size_t N, typename = void>
  struct arguments_needed
  {
    static constexpr size_t value ()
    {
      static_assert (N > 0, "Wrong slot arguments");
      return arguments_needed<Func, Tuple, N - 1>::value ();
    }
  };

  template<typename Func, typename Tuple, size_t N>
  struct arguments_needed<Func, Tuple, N, decltype (try_call (std::declval<Func> (),
                                                              std::declval<Tuple> (),
                                                              std::make_index_sequence<N> {}))>
  {
    static constexpr size_t value () {return N;}
  };


  template<typename Func, typename Tuple, size_t... I>
  void call_final (Func &f, Tuple &&tup, std::index_sequence<I...>)
  {
    f (std::get<I> (tup)...);
  }

  template<typename Func>
  struct call_helper
  {
    Func func;
    call_helper (Func &&f)
      : func (std::forward<Func> (f)) {}

    template<typename...Args>
    void operator () (Args &&... args) const
    {
      constexpr size_t N = arguments_needed<Func, std::tuple<Args...>, sizeof... (Args)>::value ();
      call_final (func, std::forward_as_tuple (std::forward<Args> (args)...), std::make_index_sequence<N> {});
    }

  };

  template<typename T, typename Ref = std::add_lvalue_reference_t<T>>
    Ref declref_helper (int);

  template<typename T>
    T declref_helper (long);

  template<typename T>
    auto declref () noexcept -> decltype (declref_helper<T> (0));


    template<typename T>
    using use_if_primitive = std::enable_if_t<std::is_fundamental_v<T>
                                              || std::is_enum_v<T>
                                              || std::is_convertible_v<T, std::string>,void>;

    template<typename T>
    struct is_primitive
    {
    private:
      template<typename C, typename = use_if_primitive<T>>
      constexpr bool is_primitive_impl (int) {return true;}

      template<typename C>
      constexpr bool is_primitive_impl (char) {return false;}
    public:
      static constexpr bool value = is_primitive_impl<T> (0);

    };

    template<typename T>
    inline constexpr bool is_primitive_v = is_primitive<T>::value;

    struct priority_tag {};

    using priority_tag_1 = priority_tag;

    struct priority_tag_2
    {
      priority_tag_2 (const priority_tag &) {}
    };

    struct priority_tag_3
    {
      priority_tag_3 (const priority_tag_2 &) {}
    };

    struct priority_tag_4
    {
      priority_tag_4 (const priority_tag_3 &) {}
    };

    struct priority_tag_5
    {
      priority_tag_5 (const priority_tag_4 &) {}
    };
}

#endif // TEMPLATE_UTILS_H
