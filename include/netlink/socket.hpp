#pragma once

#include <utility>
#include "netlink/msg.hpp"

namespace nl {

	class socket {

		public:
			void open();
			void close();
			bool is_open();
			bool is_open() const;

			int send(const nl::msg& msg) const;
			int recv() const;

			std::pair<int, int> perform(const nl::msg& msg) const;

			socket();
			~socket();

		private:
			int fd = -1;

	};
}
