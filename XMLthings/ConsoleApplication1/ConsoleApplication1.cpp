// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	xml::XMLNode *node;

	char data[] = "<test a=\"233\">test</test>";

	xml::XMLParser xp;

	xp.parse(data, strlen(data));

	node = xp.pickupDocument();
	xp.saveNode(node, &std::cout);
	delete node;
	system("pause");

	return 0;
}

