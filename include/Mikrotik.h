#ifndef MIKROTIK_HPP
#define MIKROTIK_HPP

#include "Note.h"
#include <vector>

class Mikrotik
{
public:
	Mikrotik(/* args */);
	~Mikrotik();
	void setNoteComments(bool flag);
	std::string getScriptHeader();
	std::string buildNote(Note noteOn, Note noteOff);
	int buildScript(std::string &fileName);
private:
	bool m_commentsFlag;
	std::vector<Note> m_notes;
};




#endif // MIKROTIK_HPP