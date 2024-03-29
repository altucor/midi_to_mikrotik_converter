#include "VLV.hpp"
#include "boost/log/trivial.hpp"

const static uint8_t MIDI_VLV_CONTINUATION_BIT = 0x80;
const static uint8_t MIDI_VLV_DATA_MASK = 0x7F;
const static uint8_t vlv_max_size = sizeof(uint32_t);

/*
 * Very detailed articles about VLV(Variable Length Values)
 * http://www.ccarh.org/courses/253/handout/vlv/
 * http://midi.teragonaudio.com/tech/midifile.htm
 */

VLV::VLV(ByteStream &stream)
{
	uint8_t data = 0;
	while(1)
	{
		if(m_streamCounter > vlv_max_size)
		{
			BOOST_LOG_TRIVIAL(error) << "VLV Error trying to read more than possible: " << m_streamCounter << " bytes";
			return;
		}
		m_vlv = (m_vlv << 7);
		data = stream.get8u(); // value duration
		m_streamCounter++;
		// save all lower 7 bits except high continuation bit
		m_vlv += (data & MIDI_VLV_DATA_MASK);
		if(!(data & MIDI_VLV_CONTINUATION_BIT))
			break;
	}
	if(m_streamCounter > sizeof(uint32_t))
	{
		BOOST_LOG_TRIVIAL(error) << "VLV Error readed from stream: " << m_streamCounter << " bytes";
	}
}

uint32_t VLV::getValue()
{
	return m_vlv;
}
