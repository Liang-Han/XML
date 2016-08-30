#include "stdafx.h"
#include "XMLParser.h"

using namespace std;

namespace xml
{
	namespace toolbox
	{
		DataTimer::DataTimer(DataPack & data)
			:m_pData(data)
		{
			for (int i = 0; i < max_stage; i++)
			{
				m_pPos[i] = nullptr;
			}

			m_iStage = 0;
		}

		DataTimer::~DataTimer()
		{
		}

		const char * DataTimer::getData() const
		{
			return m_pData.getData();
		}

		unsigned int DataTimer::split()
		{
			m_pPos[m_iStage] = m_pData.getData();
			return m_iStage++;
		}

		void DataTimer::reset(unsigned int count)
		{
			for (unsigned int i = 0; i < max_stage; i++)
			{
				if (i >= count)m_pPos[i] = nullptr;
				if (i < count && i >= m_iStage) m_pPos[i] = getData();
			}

			m_iStage = count;
		}

		int DataTimer::isEnd() const
		{
			return m_pData.isEnd();
		}

		size_t DataTimer::skipChar(string str)
		{
			size_t count = 0;
			for (; !isEnd(); plus())
			{
				if (str.find(*m_pData.getData()) == string::npos) return count;
				count++;
			}

			return eos;
		}

		size_t DataTimer::skipTill(string str)
		{
			size_t count = 0;
			for (; !isEnd(); plus())
			{
				if ((count = str.find(*m_pData.getData())) != string::npos) return count;
			}

			return eos;
		}

		const char * DataTimer::getData(unsigned int count) const
		{
			if (m_pPos[count] == nullptr)
				return m_pData.getData();
			else
				return m_pPos[count];
		}

		unsigned int DataTimer::getStage() const
		{
			return m_iStage;
		}

		size_t DataTimer::getDiff(unsigned int count) const
		{
			if (count == 0) return m_pData.getPosition();
			if (count >= m_iStage)
				return m_pData.getData() - m_pPos[count - 1];
			else
				return m_pPos[count] - m_pPos[count - 1];
		}

		void DataFragment::saveString(const char * start, size_t size)
		{
			m_Cache.assign(start, size);
		}

		void DataFragment::combineString(const char * start, size_t size)
		{
			m_Buffer = m_Cache;
			m_Buffer.append(start, size);
			m_Cache.clear();
		}

		FormatChecker::FormatChecker()
		{
			m_StrMap.insert(pair<string, string>("&lt;", "<"));
			m_StrMap.insert(pair<string, string>("<", "&lt;"));

			m_StrMap.insert(pair<string, string>("&gt;", ">"));
			m_StrMap.insert(pair<string, string>(">", "&gt;"));

			m_StrMap.insert(pair<string, string>("&amp;", "&"));
			m_StrMap.insert(pair<string, string>("&", "&amp;"));

			m_StrMap.insert(pair<string, string>("&apos;", "'"));
			m_StrMap.insert(pair<string, string>("'", "&apos;"));

			m_StrMap.insert(pair<string, string>("&quot;", "\""));
			m_StrMap.insert(pair<string, string>("\"", "&quot;"));

		}

		FormatChecker::~FormatChecker()
		{
		}

		string FormatChecker::getValidString(const char * source, size_t count)
		{
			string result;
			const char * start = source;
			int i;
			for (i = 0; start + i < source + count; )
			{
				if (start[i] == '\r' || start[i] == '\n') //no line breaks.
				{
					result.append(start, i);
					start = start + i + 1;
					i = 0;
				}
				else if (start[i] == 32) //mutiple space will be convert to one. To do: did we need to remove the begining space of a string?
				{
					int j;
					for (j = 1; start + i + j < source + count; j++) {
						if (start[i + j] != 32) break;
					}
					if (j > 1)
					{
						result.append(start, i);
						if (result.size() == 0) result.append(" ");
						else if (result[result.size() - 1] != 32) result.append(" ");
						start = start + i + j;
						i = 0;
					}
					else
					{
						i++; //nothing to worry
					}
				}
				else if (start[i] == '&') //& start...
				{
					string key; int j;
					for (j = 0; j < 6; j++)
					{
						if (start[i + j] == ';') {
							key.assign(start + i, j + 1);
							break;
						}
						if (start + i + j >= source + count) break;
					}
					if (key.size()) //find &XXX;
					{
						result.append(start, i);
						result.append(m_StrMap[key]);
						start = start + i + j + 1;
						i = 0;
					}
					else
					{
						i++;//error: there is a & but no ;
					}
				}
				else
				{
					i++;
				}
			}
			result.append(start, i);

			return result;
		}
		string FormatChecker::getOrignalString(xml::XMLNode *node, int isCloseTag)
		{
			string temp;

			if (isCloseTag && node->getType() != ELEMENT_NODE)
			{
				return "";
			}

			//element start
			if (node->getType() == ELEMENT_NODE)
			{
				temp += "<";
			}
			if (isCloseTag)
			{
				temp += "/";
			}

			//////
			temp += getOrignalString(node->getString().c_str(), node->getString().size());

			if (!isCloseTag)
			{
				//attributes
				const XMLNode::AttNode *att = nullptr;
				while (att = node->getAttribute(att))
				{					
					temp.append(" ").append(att->getKey()).append("=\"").append(
					getOrignalString(att->getValue().c_str(),att->getValue().size())
					).append("\"");
				}
			}
			
			if (node->getType() == ELEMENT_NODE)
			{
				temp.append(">");
			}
			return temp;
		}
		std::string FormatChecker::getOrignalString(const char * source, size_t count)
		{
			string temp; const char * start = source; int i = 0;
			while (start + i < source + count)
			{
				std::map<std::string, std::string>::iterator it = m_StrMap.find(string(1, *(start + i)));
				if (it != m_StrMap.end())
				{
					temp.append(start, i);
					temp.append(it->second);
					start = start + i + 1;
					i = 0;
				}
				else
				{
					i++;
				}
			}
			temp.append(start, i);
			return temp;
		}
	}

	using namespace toolbox;
	XMLParser::XMLParser()
	{
		m_Document = new XMLNode;
		m_Root = m_Document;

		m_Status = SUCCESS;

		m_TextStart = nullptr;
		m_TextEnd = nullptr;
	}


	XMLParser::~XMLParser()
	{
		delete m_Document;
	}

	void XMLParser::parse(char * source, size_t size)
	{
		if (m_Status == CONTINUE)
		{
			m_SavedData.combineString(source, size);
			m_Data.assign(m_SavedData.getBuffer(), m_SavedData.getBufferSize());
			m_Status = SUCCESS;
		}
		else
		{
			m_Data.assign(source, size);
		}

		while (!m_Data.isEnd())
		{
			if (*m_Data.getData() == '<')
			{
				finishText();

				if (m_Data.getSize() - m_Data.getPosition() <= 2)
				{
					m_SavedData.saveString(m_Data.getData(), m_Data.getSize() - m_Data.getPosition());
					m_Status = CONTINUE;
					break;
				}

				if (memcmp(m_Data.getData(), "<![CDATA[", 3) == 0)
				{
					if (createCDATA() != SUCCESS) break;
				}
				else if (memcmp(m_Data.getData(), "<!--", 3) == 0) // check for comments
				{
					if (createComment() != SUCCESS) break;
				}
				else if (memcmp(m_Data.getData(), "<?", 2) == 0) //make some optimization
				{
					if (createProcessingInstruction() != SUCCESS) break;
				}
				else
				{
					if (createElement() != SUCCESS) break;
				}

			}
			else
			{
				updateText(m_Root, m_Data.getData());

				m_Data.plus();
			}
		}

		if (m_TextStart != nullptr)
		{
			m_SavedData.saveString(m_TextStart, m_TextEnd - m_TextStart + 1);
			m_TextStart = nullptr;
			m_TextEnd = nullptr;

			m_Status = CONTINUE;
		}
	}

	XMLNode * XMLParser::pickupDocument()
	{
		XMLNode * temp = m_Document;
		m_Document = new XMLNode;
		m_Root = m_Document;
		return temp;

	}

	void XMLParser::saveNode(XMLNode *node, std::ostream * stdstream)
	{
		checkNode(node, 0, stdstream);
	}

	void XMLParser::clear()
	{
		delete m_Document;
		m_Document = new XMLNode;
		m_Root = m_Document;

		m_Status = SUCCESS;

		m_TextStart = nullptr;
		m_TextEnd = nullptr;
	}

	void XMLParser::checkNode(XMLNode* node, int level, ostream * stdstream)
	{
		while (node)
		{
			//for (int i = 0; i < level; i++) (*stdstream) << "   ";
			(*stdstream) << m_Checker.getOrignalString(node, false);// << endl;

			if (node->getFirstChild())
				checkNode(node->getFirstChild(), level + 1, stdstream);

			if (node->getType() == ELEMENT_NODE) {
				//for (int i = 0; i < level; i++) (*stdstream) << "   ";
				(*stdstream) << m_Checker.getOrignalString(node, true);// << endl;
			}
			node = node->getNext();
		}
	}

	Result XMLParser::createElement()
	{
		DataTimer data(m_Data);
		data.split();
		XMLNode *pNode = nullptr;
		size_t count;

		try
		{
			//make a good start
			if (*data.getData() != '<')
				return SUCCESS;
			if (data.plus()) throw CONTINUE;

			if (data.skipChar(" \n\r")) throw FAILURE; //did not expect a space

			if (*data.getData() == '/')	 //close node
			{
				if (data.plus()) return CONTINUE; //skip the "/"

				data.split();
				if (data.skipTill(">") == data.eos) throw CONTINUE;
				if (m_Root->getString().compare(0, data.getDiff(2), data.getData(1), data.getDiff(2)) != 0) throw FAILURE;

				if (data.plus()) throw CONTINUE; //skip the ">" close tag does not matter;

				if (m_Root->getParent() != nullptr)
					m_Root = m_Root->getParent(); //return the parent node
				else
					throw DONE; //finished

				return SUCCESS; //succeed
			}

			//create element
			pNode = new XMLNode;

			data.split();

			//get the name
			if ((count = data.skipTill(" \r\n>/")) == data.eos) throw CONTINUE;

			//set the new node name
			pNode->setString(data.getData(1), data.getDiff(2));
			if (m_Root != nullptr) m_Root->append(pNode);

			//hanlde it when there are spaces
			if (count < 3)
			{
				if (data.skipChar(" \r\n") == data.eos) throw CONTINUE;

				if (*data.getData() == '>') throw FAILURE;
				if (*data.getData() != '/')
				{
					while (true)
					{
						Result attError = createAttribute(pNode);
						if (attError != SUCCESS) throw attError;

						if (count = data.skipChar(" \r\n"))
						{
							if (count == data.eos) throw CONTINUE;
							if (*data.getData() == '>' || *data.getData() == '/') break; //end of tag

						}
						else
						{
							if (*data.getData() == '>' || *data.getData() == '/') break; //end of tag
							throw FAILURE;
						}
					}
				}
			}

			if (*(data.getData()) == '/')
			{
				if (data.plus()) throw CONTINUE;

				if (*data.getData() != '>') throw FAILURE;
				if (m_Root == nullptr) m_Root = pNode; //empty nood is the root. error handle: this means finished
			}
			else
			{
				m_Root = pNode;
			}

			if (data.plus()) throw CONTINUE; //skip the '>'
		}
		catch (Result error)
		{
			if (pNode != nullptr)
			{
				if (m_Root == pNode) m_Root = pNode->getParent();
				delete pNode;
			}
			if (error == CONTINUE)
			{
				m_SavedData.saveString(data.getData(0), data.getData() - data.getData(0));
			}

			m_Status = error;
			return error;
		}

		return SUCCESS;
	}

	Result XMLParser::createAttribute(XMLNode * parent)
	{
		DataTimer data(m_Data);
		data.split(); //no usage
		string name, value;

		size_t count;

		try
		{
			data.split(); //mark name start

			count = data.skipTill(" \r\n=>");
			if (count == data.eos) throw CONTINUE;
			if (count == 4) throw FAILURE;

			//set the name		
			name.assign(data.getData(1), data.getDiff(2));

			if (count < 3)
			{
				if (data.skipChar(" \r\n") == data.eos) throw CONTINUE; //clear the spaces before =
			}

			if (*data.getData() != '=') throw FAILURE;//error handle: there are some spaces inside the name

			if (data.plus()) throw CONTINUE; //skip the '='

			if (data.skipChar(" \r\n") == data.eos) throw CONTINUE; //skip spaces.

			if (*data.getData() != '\"') throw FAILURE; //error handle: the value did not start with "

			if (data.plus()) throw CONTINUE; //skip the "
			data.split(); //mark the value start

			count = data.skipTill("\">");
			if (count == 1) throw FAILURE;
			if (count == data.eos) throw CONTINUE;

			value = m_Checker.getValidString(data.getData(2), data.getDiff(3));
			parent->setAttribute(name, value);

			if (data.plus()) throw CONTINUE; //skip the "
		}
		catch (Result error)
		{

			m_Status = error;
			return error;
		}

		return SUCCESS;
	}


	Result XMLParser::createComment()
	{
		DataTimer data(m_Data);
		data.split();
		XMLNode * pNode = nullptr;

		try
		{
			for (int i = 0; i < 4; i++)
			{
				if (*data.getData() != "<!--"[i]) throw FAILURE;
				if (data.plus()) throw CONTINUE;
			}

			if (data.skipChar(" ") == data.eos) throw CONTINUE;

			data.split();

			while (true)
			{
				if (*data.getData() == '-')
				{
					if (data.plus()) throw CONTINUE;
					if (*data.getData() == '-')
					{
						data.split();
						break;
					}
				}

				if (data.plus()) throw CONTINUE;
			}

			if (data.plus()) throw CONTINUE;

			if (*data.getData() != '>') throw FAILURE;

			const char * endChar;
			for (endChar = data.getData(2) - 2; endChar > data.getData(1); endChar--)
			{
				if (*endChar != ' ') break;
			}

			pNode = new XMLNode; pNode->convertType(COMMENT_NODE);

			pNode->setString(data.getData(1), endChar - data.getData(1) + 1);

			m_Root->append(pNode);

			if (data.plus()) throw CONTINUE;

		}
		catch (Result error)
		{
			if (pNode != nullptr)
			{
				delete pNode;
			}

			if (error == CONTINUE)
			{
				m_SavedData.saveString(data.getData(0), data.getData() - data.getData(0));
			}

			m_Status = error;
			return error;
		}

		return SUCCESS;
	}

	Result XMLParser::createProcessingInstruction()
	{
		DataTimer data(m_Data);
		data.split();
		XMLNode * pNode = nullptr;

		try
		{
			for (int i = 0; i < 2; i++)
			{
				if (*data.getData() != "<?"[i]) throw FAILURE;
				if (data.plus()) throw CONTINUE;
			}

			data.split();

			while (true)
			{
				if (*data.getData() == '>')
				{
					if (*(data.getData() - 1) == '?')
					{
						break;
					}
					else
					{
						throw FAILURE;
					}
				}

				if (data.plus()) throw CONTINUE;
			}

			pNode = new XMLNode; pNode->convertType(PROCESSING_INSTRUCTION_NODE);
			pNode->setString(data.getData(1), data.getDiff(2) - 1);
			m_Root->append(pNode);

			if (data.plus()) throw CONTINUE;
		}
		catch (Result error)
		{
			if (pNode != nullptr)
			{
				delete pNode;
			}

			if (error == CONTINUE)
			{
				m_SavedData.saveString(data.getData(0), data.getData() - data.getData(0));
			}

			m_Status = error;
			return error;
		}

		return SUCCESS;
	}

	Result XMLParser::createCDATA()
	{
		DataTimer data(m_Data);
		data.split();
		XMLNode * pNode = nullptr;

		try
		{
			for (int i = 0; i < 9; i++)
			{
				if (*data.getData() != "<![CDATA["[i]) throw FAILURE;
				if (data.plus()) throw CONTINUE;
			}

			data.split();

			while (true)
			{
				if (*data.getData() == '>')
				{
					if (memcmp(data.getData() - 2, "]]>", 3) == 0)
					{
						break;
					}
				}

				if (data.plus()) throw CONTINUE;
			}

			pNode = new XMLNode; pNode->convertType(CDATA_SECTION_NODE);
			pNode->setString(data.getData(1), data.getDiff(2) - 2);
			m_Root->append(pNode);

			if (data.plus()) throw CONTINUE;
		}
		catch (Result error)
		{
			if (pNode != nullptr)
			{
				delete pNode;
			}

			if (error == CONTINUE)
			{
				m_SavedData.saveString(data.getData(0), data.getData() - data.getData(0));
			}

			m_Status = error;
			return error;
		}

		return SUCCESS;
	}

	size_t XMLParser::finishText()
	{
		if (m_Root != nullptr && m_TextStart != nullptr)
		{
			string &text = m_Checker.getValidString(m_TextStart, m_TextEnd - m_TextStart + 1);
			m_TextEnd = nullptr;
			m_TextStart = nullptr;

			if (text.size() == 0)
			{
				return 0;
			}

			XMLNode * pNode = new XMLNode; pNode->convertType(TEXT_NODE);

			pNode->setString(text.c_str(), text.size());

			m_Root->append(pNode);
			return text.size();
		}
		else
		{
			return 0;
		}
	}

	void XMLParser::updateText(XMLNode *parent, const char * data)
	{
		if (m_TextStart == nullptr)
		{
			m_TextStart = data;
		}
		m_TextEnd = data;
	}
}