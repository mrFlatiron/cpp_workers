#ifndef XML_WORKER_H
#define XML_WORKER_H

#include "workers.h"
#include "internal/template_utils.h"

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
    xmlDocPtr m_doc = nullptr;
    xmlNodePtr m_current_node = nullptr;
    bool m_is_ok = true;

    bool m_action_completed = false;
    std::string m_file_name;
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
      if (!node->children || !node->children->content)
        {
          m_is_ok = false;
          return nullptr;
        }

      return reinterpret_cast<const char *> (node->children->content);
    }


  public:

    xml_worker (action_t act, std::string file_name = "-")
    {
      m_action = act;
      m_file_name = std::move (file_name);

      switch (m_action)
        {
        case action_t::save:
          {
            m_doc = xmlNewDoc (BAD_CAST ("1.0"));
            if (m_doc)
              {
                m_current_node = xmlNewNode (nullptr, BAD_CAST ("root"));
                xmlDocSetRootElement (m_doc, m_current_node);
              }
            break;
          }
        case action_t::load:
          {
            m_doc = xmlReadFile (m_file_name.c_str (), "UTF-8", 0);
            m_is_ok = m_doc;
            if (m_doc)
              {
                m_current_node = xmlDocGetRootElement (m_doc);
                if (!m_current_node)
                  {
                    m_is_ok = false;
                    break;
                  }
                break;
              }
          }
        }
    }

    ~xml_worker ()
    {
      if (m_action_completed)
        return;

      finalize ();
    }

    void set_next_name (const std::string &name)
    {
      if (!m_doc)
        return;

      switch (m_action)
        {
        case action_t::save:
          {
            m_current_node = xmlNewChild (m_current_node,
                                          nullptr,
                                          BAD_CAST (name.c_str ()),
                                          nullptr);
            break;
          }
        case action_t::load:
          {
            m_current_node = get_first_child_node (m_current_node, name.c_str ());
            break;
          }
        }
    }

    void back ()
    {
      auto node = m_current_node;
      if (m_current_node)
        {
          m_current_node = m_current_node->parent;

          if (m_action == action_t::load)
            {
              xmlUnlinkNode (node);
              xmlFreeNode (node);
            }
        }
    }

    bool is_ok () const {return m_is_ok;}
    action_t action () const {return m_action;}

    template<typename T, typename = work::use_if_primitive<T>>
    bool process_base (T &obj)
    {
      if (!m_current_node)
        {
          m_is_ok = false;
          return false;
        }
      if constexpr (std::is_fundamental_v<T>)
        {
        switch (m_action)
          {
          case action_t::save:
            xmlNewChild (m_current_node,
                         nullptr,
                         BAD_CAST ("value"),
                         BAD_CAST (to_string_enh (obj).c_str ()));
            break;
          case action_t::load:
            {
              auto value_node = get_first_child_node (m_current_node, "value");
              if (!value_node)
                {
                  m_is_ok = false;
                  return false;
                }
              auto content_ptr = get_node_content (value_node);
              if (!content_ptr)
                {
                  m_is_ok = false;
                  return false;
                }
              obj = from_string<T> (content_ptr);
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
                {
                  auto node = get_first_child_node (m_current_node, "value");
                  if (!node)
                    {
                      m_is_ok = false;
                      return false;
                    }
                  auto content_ptr = get_node_content (node);
                  if (!content_ptr)
                    {
                      m_is_ok = false;
                      return false;
                    }
                  obj = static_cast<T> (std::string (content_ptr));
                }
                break;
              }
            }
          else
            {
              static_assert (std::is_enum_v<T>, "unknown base type");
              switch (m_action)
                {
                case action_t::save:
                  xmlNewChild (m_current_node,
                               nullptr,
                               BAD_CAST ("value"),
                               BAD_CAST (enum_to_string (obj).c_str ()));
                  break;
                case action_t::load:
                  {
                    auto node = get_first_child_node (m_current_node, "value");
                    if (!node)
                      {
                        m_is_ok = false;
                        return false;
                      }
                    auto content_ptr = get_node_content (node);
                    if (!content_ptr)
                      {
                        m_is_ok = false;
                        return false;
                      }
                    obj = string_to_enum<T> (content_ptr);
                  }
                  break;
                }
            }
        }
      return true;
    }
      bool finalize ()
      {
        if (m_action_completed)
          return true;

        switch (m_action)
          {
          case action_t::save:
            if (m_doc)
              if (xmlSaveFormatFileEnc (m_file_name.c_str (), m_doc, "UTF-8", 1))
                return false;
            break;
          case action_t::load:
            break;
          }

        if (m_doc)
          xmlFreeDoc (m_doc);

        xmlCleanupParser ();

        return true;
      }
  };


}

#endif // XML_WORKER_H
