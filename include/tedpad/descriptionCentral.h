#pragma once

#include "../../engine/include/eg/eg_engine.h"

namespace tedpad {
	namespace Key {
		enum e {
			DataReference_Description,
			Module,
			Attribute,
		};
	}
	namespace Value {
		namespace DataReference_Description {
			enum e {
				File,
			};
		}
		namespace Module {
			enum e {
				Invalid,
				Attribute_Generic,
				Attribute_PlaceHolder,
				Attribute_Digital,
				Attribute_Analogue,
				Attribute_Axis,
				Attribute_Buffer,
				GamepadBriefDescription,
				GamepadFullDescription,
				GamepadData,
			};
		}
	}
}
