#pragma once

#include <inttypes.h>
#include <string>
#include <vector>
#include "../../../engine/include/eg/eg_engine.h"

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
