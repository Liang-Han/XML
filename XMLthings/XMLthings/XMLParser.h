#pragma once
#include "XMLNode.h"
#include <map>

namespace xml
{
	namespace toolbox //the classes help to parse xml
	{
		class DataPack
		{
		public:
			DataPack() { assign(nullptr, 0); }
			~DataPack() {}

			//get point
			inline const char* getData() {
				return m_Data + m_Pos;
			}

			inline size_t getPosition() {
				return m_Pos;
			}

			inline size_t getSize() {
				return m_Size;
			}

			inline int isEnd() {
				return (m_Pos >= m_Size);
			}

			//if it reach the end, return 1
			inline int plus(size_t count = 1) {
				m_Pos += count;
				return isEnd();
			}

			//assign char pointer
			void assign(const char *source, size_t size) {
				m_Data = source;
				m_Size = size;
				m_Pos = 0;
			}

		private:
			const char *m_Data; //data
			size_t m_Size;
			size_t m_Pos;
		};

		class DataTimer
		{
		public:
			DataTimer(DataPack & data);
			~DataTimer();

			static const size_t eos = (size_t)(-1);
			static const size_t max_stage = 5;

			const char* getData() const;

			//add a stage.
			unsigned int split();

			//reset to one stage and clear later ones.
			void reset(unsigned int count = 0);

			//if it reach the end, return 1
			inline int plus(size_t count = 1) { return m_pData.plus(count); }

			int isEnd()const;

			//skip char c. if it is the end, return false.
			size_t skipChar(std::string str);
			//skip until char c. if it is the end, return false.
			size_t skipTill(std::string str);

			const char* getData(unsigned int count)const;
			unsigned int getStage()const;
			size_t getDiff(unsigned int count)const;

		private:
			DataPack & m_pData;
			const char * m_pPos[max_stage];  //pointer
			unsigned int m_iStage;  //which pointer

		};

		class DataFragment
		{
		public:
			void saveString(const char * start, size_t size);
			void combineString(const char * start, size_t size);

			const char * getBuffer() { return m_Buffer.c_str(); }
			size_t getBufferSize() { return m_Buffer.size(); }

		private:
			std::string m_Buffer;
			std::string m_Cache;

		};

		class FormatChecker
		{
		public:
			FormatChecker();
			~FormatChecker();
			
			std::string getValidString(const char* source, size_t count);
			std::string getOrignalString(xml::XMLNode *node, int isCloseTag);
			std::string getOrignalString(const char* source, size_t count);

		private:
			std::map<std::string, std::string> m_StrMap;
		};
	}

	class XMLParser
	{
	public:
		XMLParser();
		~XMLParser();

		void parse(char * source, size_t size);

		Result getStatus() { return m_Status; }

		const XMLNode * getDocument() { return m_Document; }
		XMLNode * pickupDocument();
		void saveNode(XMLNode *node, std::ostream * stdstream);

		void clear();

		toolbox::FormatChecker m_Checker;

	private:
		size_t finishText();
		void updateText(XMLNode *parent, const char * data);

		const char *m_TextStart, *m_TextEnd;

		toolbox::DataPack m_Data;

		XMLNode * m_Root;
		XMLNode * m_Document;
		toolbox::DataFragment m_SavedData;

		Result m_Status;

		virtual Result createElement();
		virtual Result createAttribute(XMLNode * parent);
		virtual Result createComment();
		virtual Result createProcessingInstruction();
		virtual Result createCDATA();

		void checkNode(xml::XMLNode* node, int level, std::ostream * stdstream);
	};
}
