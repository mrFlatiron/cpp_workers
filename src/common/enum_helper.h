#ifndef ENUM_HELPER_H
#define ENUM_HELPER_H


template<typename Enum>
using use_if_enum = typename std::enable_if<std::is_enum<Enum>::value, Enum>::type;



template<typename Enum, typename = use_if_enum<Enum>>
class enum_helper
{
private:
  unsigned int m_pos;
public:
  enum_helper (Enum init) { m_pos = static_cast<unsigned int> (init); }

  enum_helper<Enum> &operator++ ()                      { m_pos++; return *this; }
  bool operator == (const enum_helper<Enum> &rhs) const { return m_pos == rhs.m_pos; }
  bool operator != (const enum_helper<Enum> &rhs) const { return m_pos != rhs.m_pos; }
  Enum operator *  ()                             const { return static_cast<Enum> (m_pos); }
};

template<typename Enum, typename = use_if_enum<Enum>>
constexpr auto enum_begin (Enum, Enum = Enum ()) -> decltype (Enum ()) {return static_cast<Enum> (0);}

template<typename Enum, typename = use_if_enum<Enum>>
constexpr auto enum_end (Enum, Enum = Enum ()) -> decltype (Enum::COUNT) {return Enum::COUNT;}

#endif // ENUM_HELPER_H
