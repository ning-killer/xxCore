#pragma once

class PipeWrap
{
public:
	PipeWrap();
	~PipeWrap();

	int write(const void* buf, int n);
	int read(void* buf, int n);

	int readFD() const {
		return _pipe_fd[0];
	}
private:
	int _pipe_fd[2] = { -1,-1 };
#ifdef _WIN32
	int _listenerFd = -1;
#endif 
};

