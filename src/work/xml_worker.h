#ifndef XML_WORKER_H
#define XML_WORKER_H

#include "workers.h"
#include "common/utils.h"

#include <libxml/tree.h>
#include <vector>
#include <cstdio>
#include <string_view>

namespace work
{
  class xml_worker
  {
  private:
    action_t m_action = action_t::save;

    xmlNodePtr m_current_node = nullptr;
    bool m_is_ok = true;

    std::string m_file_name;
    FILE *m_out = nullptr;
  private:
    xmlNodePtr get_first_child_node (xmlNodePtr node, const char *name)
    {
      xmlNodePtr cur_node = node->children;
      for (cur_node = node->children; cur_node; cur_node = cur_node->next)
        if (cur_node->type == XML_ELEMENT_NODE && !xmlStrcmp (reinterpret_cast<const unsigned char *> (name), cur_node->name))
          break;

      return cur_node;
    }

    const char *get_node_content (xmlNodePtr node)
    {
        return reinterpret_cast<const char *> (node->children->content);
    }


  public:
    xmlDocPtr m_doc = nullptr;
    xml_worker (action_t act, std::string file_name = "-");
    ~xml_worker ();

    void set_next_name (const std::string &name);
    void back ();
    bool is_ok () const {return true;}
    action_t action () const {return m_action;}

    template<typename T, typename = templ::use_if_primitive<T>>
    bool process_base (T &obj)
    {
      if constexpr (std::is_fundamental_v<T>)
        {
        switch (m_action)
          {
          case action_t::save:
            xmlNewChild (m_current_node,
                         nullptr,
                         BAD_CAST ("value"),
                         BAD_CAST (std::to_string (obj).c_str ()));
            break;
          case action_t::load:
            {
              auto value_node = get_first_child_node (m_current_node, "value");
              std::cout << value_node->name << std::endl;
              obj = from_string<T> (get_node_content (value_node));
            }
          }
        }
      else
        {
          if constexpr (std::is_convertible_v <T, std::string> && std::is_convertible_v<std::string, T>)
            {
            switch (m_action)
              {
              case action_t::save:
                xmlNewChild (m_current_node,
                             nullptr,
                             BAD_CAST ("value"),
                             BAD_CAST (static_cast<std::string> (obj).c_str ()));
                break;
              case action_t::load:
                obj = static_cast<T> (std::string (get_node_content (get_first_child_node (m_current_node, "value"))));
                break;
              }
            }
          else
            {
              static_assert (std::is_enum_v<T>, "unknown fundamental type");
              switch (m_action)
                {
                case action_t::save:
                  xmlNewChild (m_current_node,
                               nullptr,
                               BAD_CAST ("value"),
                               BAD_CAST (enum_to_string (obj).c_str ()));
                  break;
                case action_t::load:
                  obj = string_to_enum<T> (get_node_content (get_first_child_node (m_current_node, "value")));
                  break;
                }
            }
        }
      return true;
    }

//    template<typename T>
//    bool process_base (std::vector<T> &vec)
//    {
//      xmlNewChild (m_current_node,
//                   nullptr,
//                   BAD_CAST ("size"),
//                   BAD_CAST (std::to_string (vec.size ()).c_str ()));

//      for (size_t i = 0; i < vec.size (); i++)
//        {
//          work::vector_element<T> el (i, vec[i]);
//          work::process (*this, el, "el");
//        }

//      return true;
//    }

  };
}

#endif // XML_WORKER_H
