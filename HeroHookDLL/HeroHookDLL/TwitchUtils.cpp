#include "TwitchUtils.h"
#include "GUI.h"
#include <WinBase.h>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void TwitchUtils::SetupIPC()
{
	GUI::LogToConsole("Creating Pipe");
	HANDLE pipe = CreateNamedPipe(
		"\\\\.\\pipe\\my_pipe", // name of the pipe
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE, // send data as a byte stream
		1, // only allow 1 instance of this pipe
		0, // no outbound buffer
		0, // no inbound buffer
		0, // use default wait time
		NULL // use default security attributes
	);
	if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
		GUI::LogToConsole("Pipe Broke");
		return;
	}

	BOOL result = ConnectNamedPipe(pipe, NULL);
	if (!result) {
		GUI::LogToConsole("Pipe Broke");
	}

	open("");

	GUI::LogToConsole("Got Message");

	GUI::LogToConsole("Closed Pipe");

	CloseHandle(pipe);

}