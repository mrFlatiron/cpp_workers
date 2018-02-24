#ifndef TYPESAFE_ID_H
#define TYPESAFE_ID_H



template<typename index_t, typename IdName>
class typesafe_id_generic
{
private:
  using self = typesafe_id_generic<index_t, IdName>;
  index_t m_id;
public:
  typesafe_id_generic () : m_id () {}

  self operator++ (int) {self temp = *this; m_id++; return temp;}
  self operator++ ()    {++m_id; return *this;}
  explicit operator index_t () const {return static_cast<index_t> (m_id);}
  bool operator == (self rhs) const {return m_id == rhs.m_id;}
  bool operator != (self rhs) const {return m_id != rhs.m_id;}
  bool operator <  (self rhs) const {return m_id < rhs.m_id;}
  bool operator >  (self rhs) const {return m_id > rhs.m_id;}
  bool operator <= (self rhs) const {return m_id <= rhs.m_id;}
  bool operator >= (self rhs) const {return m_id >= rhs.m_id;}

  bool operator == (int rhs) const {return m_id == rhs;}
  bool operator != (int rhs) const {return m_id != rhs;}
  bool operator <  (int rhs) const {return m_id < rhs;}
  bool operator >  (int rhs) const {return m_id > rhs;}
  bool operator <= (int rhs) const {return m_id <= rhs;}
  bool operator >= (int rhs) const {return m_id >= rhs;}

  using base = index_t;

};

namespace std
{
  template<typename index_t, typename IdName>
  struct hash<typesafe_id_generic<index_t, IdName>>
  {
    std::size_t operator() (const typesafe_id_generic<index_t, IdName>& k) const
    {
      return std::hash<index_t> ().operator () (static_cast<index_t> (k));
    }
  };
}

#define MAKE_TYPESAFE_ID(NEWIDNAME, base) \
  class NEWIDNAME##_maketypesafeid_internal {}; \
  using NEWIDNAME = typesafe_id_generic<base, NEWIDNAME##_maketypesafeid_internal>

#endif // TYPESAFE_ID_H
