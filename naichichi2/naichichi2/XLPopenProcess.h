#pragma once

class XLPopenProcess
{
public:
	XLPopenProcess();
	XLPopenProcess(const std::string & processName);
	virtual ~XLPopenProcess();

	void Open(const std::string & processName);
	void IsClose();
	void Exit();
	int getExitCode();

	std::string Read();
	void Write(const std::string& data);
};