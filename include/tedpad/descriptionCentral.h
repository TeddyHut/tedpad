#pragma once

#include "../../engine/include/eg/eg_engine.h"

namespace tedpad {
#include "../../../engine/include/eg/macro/rm_w4307_0.h"
	namespace Key {
		enum e {
			DataReference_Description = murmur3_32_value("tedpad::Key::DataReference_Description", 38, 0),
			Module = murmur3_32_value("tedpad::Key::Module", 19, 0),
			Attribute = murmur3_32_value("tedpad::Attribute::Key::Attribute", 33, 0),
		};
	}
	namespace Value {
		namespace DataReference_Description {
			enum e {
				File = murmur3_32_value("tedpad::Value::DataReference_Description::File", 46, 0),
			};
		}
		namespace Module {
			enum e {
				Invalid = murmur3_32_value("tedpad::Value::Module::Invalid", 30, 0),
				Attribute_Generic = murmur3_32_value("tedpad::Value::Module::Attribute_Generic", 40, 0),
				Attribute_PlaceHolder = murmur3_32_value("tedpad::Value::Module::Attribute_PlaceHolder", 44, 0),
				Attribute_Digital = murmur3_32_value("tedpad::Value::Module::Attribute_Digital", 40, 0),
				Attribute_Analogue = murmur3_32_value("tedpad::Value::Module::Attribute_Analogue", 41, 0),
				Attribute_Axis = murmur3_32_value("tedpad::Value::Module::Attribute_Axis", 37, 0),
				Attribute_Buffer = murmur3_32_value("tedpad::Value::Module::Attribute_Buffer", 39, 0),
				GamepadBriefDescription = murmur3_32_value("tedpad::Value::Module::GamepadBriefDescription", 46, 0),
				GamepadFullDescription = murmur3_32_value("tedpad::Value::Module::GamepadFullDescription", 45, 0),
			};
		}
	}
#include "../../../engine/include/eg/macro/rm_w4307_1.h"
}
