#include "Logger.h"

void Logger::toLog(const std::string& text) {
	sd.lock();
	std::fstream out("log.txt", std::ios::app);
	if(out.is_open())
		out << text << std::endl;
	sd.unlock();
}

std::string Logger::fromLog() {
	sd.lock();
	std::fstream in("log.txt");
	std::string text;
	std::string word;
	std::getline(in, word);
	while (word != "") {
		word.clear();
		std::getline(in, word);
		text = text + word + '\n';
		std::ios::app - '\n';
	}
	sd.unlock();
	return text;
}
