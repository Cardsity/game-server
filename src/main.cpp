#include "Server.h"
#include "Utils.h"

int main(int argc, char** args)
{
#ifdef _WIN32
	srand((unsigned)time(NULL) * _getpid() + GetTickCount());
#else
	srand((unsigned)time(NULL) * getpid() + GetTickCountMs());
#endif
	server.run();
}