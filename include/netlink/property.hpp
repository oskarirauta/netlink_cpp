#pragma once

#include <utility>
#include "netlink/attr.hpp"

namespace nl {

	class property : public std::pair<int, nl::attr> {

		public:

			using pair::pair;

			int type();
			nl::attr& value();

			property& operator =(const nl::attr& attr);
			property(int type) : std::pair<int, nl::attr>(type, nullptr) {}

	};

}
