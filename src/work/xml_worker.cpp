#include "xml_worker.h"
#include "common/debug_utils.h"


work::xml_worker::xml_worker (action_t act, std::string file_name)
{
  m_action = act;
  m_file_name = std::move (file_name);

  switch (m_action)
    {
    case action_t::save:
      {
        if (m_file_name == "-")
          m_out = stdout;
        else
          {
            if (!(m_out = fopen (m_file_name.c_str (), "w")))
              {
                DEBUG_PAUSE ("Failed to open a file");
                m_is_ok = false;
                return;
              }
          }
        m_doc = xmlNewDoc (BAD_CAST ("1.0"));
        break;
      }
    case action_t::load:
      {
        m_doc = xmlReadFile (m_file_name.c_str (), "UTF-8", 0);
        m_is_ok = m_doc;
        break;
      }
    }
}

work::xml_worker::~xml_worker ()
{
  switch (m_action)
    {
    case action_t::save:
      if (m_doc)
        xmlSaveFormatFileEnc (m_file_name.c_str (), m_doc, "UTF-8", 1);
      break;
    case action_t::load:
      break;
    }

  if (m_doc)
    xmlFreeDoc (m_doc);

  xmlCleanupParser ();
}

void work::xml_worker::set_next_name (const std::string &name)
{
  if (!m_doc)
    return;

  switch (m_action)
    {
    case action_t::save:
      {
        if (!m_current_node)
          {
            m_current_node = xmlNewNode (nullptr, BAD_CAST (name.c_str ()));
            xmlDocSetRootElement (m_doc, m_current_node);
          }
        else
          {
            m_current_node = xmlNewChild (m_current_node,
                                          nullptr,
                                          BAD_CAST (name.c_str ()),
                                          nullptr);
          }
        break;
      }
    case action_t::load:
      {
        if (!m_current_node)
          {
            m_current_node = xmlDocGetRootElement (m_doc);
          }
        else
          {
            m_current_node = get_first_child_node (m_current_node, name.c_str ());
            std::cout << m_current_node->name << std::endl;
          }
        break;
      }
    }
}

void work::xml_worker::back ()
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
