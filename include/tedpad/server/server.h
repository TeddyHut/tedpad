#pragma once

#include <inttypes.h>
#include <string>
#include <vector>
#include "../../../engine/include/eg/eg_engine.h"

#include "../structures/client_description.h"

namespace tedpad {
	class Server : public eg::FullObject {
		//Public API ideas
	public:
		enum class Config_e : size_t {
			//Enables broadcasting of packets describing the server to any possible clients
			Broadcast_Enable = 0,
			AutoAccept_Enable,
		};
		enum class Status_e : size_t {
			Client_Pending = 0,
		};
		//
		//BLOCKING: Starts all server operation (starts threads, opens sockets, etc)
		void server_start();
		//BLOCKING: Stops all server operation (will block)
		void server_stop();

		//Get the data of the specified attribute. Returns 0 for success.
		//BLOCKING: Get a digital button attribute
		bool server_Get_attribute(std::string const &attribute, bool &out) const;
		//BLOCKING: Get an analogue button attribute
		bool server_Get_attribute(std::string const &attribute, uint8_t &out) const;
		//BLOCKING: Get an axis attribute
		bool server_Get_attribute(std::string const &attribute, int16_t &out) const;
		//BLOCKING: Get a buffer attribute
		bool server_Get_attribute(std::string const &attribute, uint8_t const * &out) const;
		//Set the data of the specified attribute. Rrturns 0 for success.
		//BLOCKING: Set a digital button attribute
		bool server_Set_attribute(std::string const &attribute, bool const in);
		//BLOCKING: Set an analogue button attribute	
		bool server_Set_attribute(std::string const &attribute, uint8_t const in);
		//BLOCKING: Set an axis attribute
		bool server_Set_attribute(std::string const &attribute, int16_t const in);
		//BLOCKING: Set a buffer attribute
		bool server_Set_attribute(std::string const &attribute, uint8_t const * const in);

		//Load a file descriping the gamepad that the server is serving
		bool server_Load_descriptionFile(std::string const &path);
		
		bool server_Write_descriptionFile();	//Should default to the path of the previously loaded file
		bool server_Write_descriptionFile(std::string const &path);

		eg::Param<Config_e> server_config;
		eg::Param<Status_e> server_status;

		//Vector of connected clients
		std::vector<ClientDescription> server_connectedClient;
	};
}
