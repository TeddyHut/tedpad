#pragma once

#include <inttypes.h>
#include <string>
#include <memory>
#include "../../../engine/include/eg/eg_engine.h"

#include "../socket.h"
#include "../packet/packetModule.h"
#include "../moduleName.h"
#include "moduleBase.h"

namespace tedpad {
	namespace Module {
		namespace Attribute {
			enum class PlaceHolderType : uint8_t {
				Digital = 0,
				Analogue = 1,
				Axis = 2,
				Buffer = 3,
			};
			enum class DataDirection : uint8_t {
				ServerOutput = 0,
				ServerInput = 1,
			};
			//Structure: Module( dataDirection (1) -> attributeName (?) -> attribute_specific_data (?) )
			struct Generic : public ModuleBase {
				PacketModule to_packetModule() const override;
				void from_packetModule(PacketModule const &p0) override;

				template <typename value_t>
				void set_value(value_t const &p0);
				template <typename value_t>
				value_t get_value() const;

				//Name of button (eg "Start")
				std::string attributeName;
				DataDirection direction = DataDirection::ServerOutput;
				void *value_ptr = nullptr;

				Generic();
				virtual ~Generic();
			protected:
				virtual std::vector<uint8_t> get_packetModuleData() const = 0;
				virtual void set_packetModuleData(std::vector<uint8_t> const &p0) = 0;
			};
			
			struct PlaceHolder : public Generic {
				PlaceHolderType value = PlaceHolderType::Buffer;

				operator tedpad::Module::Attribute::PlaceHolderType() const;
				PlaceHolder &operator=(PlaceHolderType const p0);
				PlaceHolder();
			protected:
				std::vector<uint8_t> get_packetModuleData() const override;
				void set_packetModuleData(std::vector<uint8_t> const &p0) override;
			};

			struct Digital : public Generic {
				bool value = false;

				operator bool() const;
				Digital &operator=(bool const p0);
				Digital();
			protected:
				std::vector<uint8_t> get_packetModuleData() const override;
				void set_packetModuleData(std::vector<uint8_t> const &p0) override;
			};

			struct Analogue : public Generic {
				uint8_t value = 0;

				operator uint8_t() const;
				Analogue &operator=(uint8_t const p0);
				Analogue();
			protected:
				std::vector<uint8_t> get_packetModuleData() const override;
				void set_packetModuleData(std::vector<uint8_t> const &p0) override;
			};

			struct Axis : public Generic {
				int16_t value = 0;

				operator int16_t() const;
				Axis &operator=(int16_t const p0);
				Axis();
			protected:
				std::vector<uint8_t> get_packetModuleData() const override;
				void set_packetModuleData(std::vector<uint8_t> const &p0) override;
			};

			struct Buffer : public Generic {
				std::vector<uint8_t> value;

				operator uint8_t *();
				operator uint8_t const *() const;
				Buffer();
			protected:
				std::vector<uint8_t> get_packetModuleData() const override;
				void set_packetModuleData(std::vector<uint8_t> const &p0) override;
			};
			template<typename value_t>
			inline void Generic::set_value(value_t const & p0)
			{
				*static_cast<value_t *>(value_ptr) = p0;
			}
			template<typename value_t>
			inline value_t Generic::get_value() const
			{
				return(*static_cast<value_t *>(value_ptr));
			}
		}
	}
}
