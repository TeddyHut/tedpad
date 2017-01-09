#pragma once

#include <fstream>

#include "../../../engine/include/eg/eg_engine.h"

#include "../descriptionCentral.h"

namespace tedpad {
	namespace DataManipulator {
		class File : public eg::DataManipulator {
		public:
			File();
		protected:
			bool loadData_impl(eg::GlbRtrn &rtrn, eg::DataReference &dataReference);
			bool freeData_impl(eg::GlbRtrn &rtrn, eg::DataReference &dataReference);
			bool writeData_impl(eg::GlbRtrn &rtrn, eg::DataReference const &dataReference);
		};
	}
}
