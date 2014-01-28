#include<rapidxml.hpp>


#define XML_HELPERS 1


namespace xml_helpers
	{

	// count the children nodes of parent
	int countChildren(rapidxml::xml_node<>* parent)
		{
		int i = 0; 
		for (rapidxml::xml_node<>* node = parent->first_node(); node; node = node->next_sibling())
			{
			i++;
			}
		return i;

		}
	bool isFirstChild(rapidxml::xml_node<>* node, const char* name = 0)
		{
		return (node->previous_sibling(name) == 0);
		}
	bool isLastChild(rapidxml::xml_node<>* node, const char* name = 0)
		{
		return (node->next_sibling(name) == 0);
		}

	}