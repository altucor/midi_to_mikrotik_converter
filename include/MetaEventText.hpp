#ifndef META_EVENT_TEXT
#define META_EVENT_TEXT

#include "Event.hpp"

class MetaEventText
{
public:
	MetaEventText(Event &event);
	std::string getText();
private:
	std::string m_text;
};

#endif // META_EVENT_TEXT
