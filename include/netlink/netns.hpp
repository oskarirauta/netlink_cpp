#pragma once

#include <string>

namespace nl {

	struct netns {

			netns& operator =(const netns& other);

			bool operator ==(const netns& other) const;
			bool operator ==(const std::string& filename) const;
			bool operator ==(int fd) const;

			operator std::string() const;
			operator int() const;

			bool is_open();
			bool is_open() const;

			void open();
			void close();
			std::string filename() const;
			int fd() const;

			netns(pid_t pid);
			netns(const std::string& filename) : _filename(filename) {};
			~netns();

		private:

			std::string _filename;
			int _fd = -1;
	};

}

std::ostream& operator <<(std::ostream& os, const nl::netns& netns);
