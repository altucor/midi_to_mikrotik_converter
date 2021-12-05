#include "MetaEventText.hpp"

MetaEventText::MetaEventText(Event &event)
{
	std::vector<uint8_t> data = event.getData();
	m_text = std::string(data.begin(), data.end());
}

std::string MetaEventText::getText()
{
	return m_text;
}
