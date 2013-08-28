#include "navdatamanager.h"
#include "../../afconstants.h"
#include <boost/bind.hpp>
#include <iostream>

using namespace std;
using boost::asio::ip::udp;

NavdataManager::NavdataManager()
{
	
}

NavdataManager::~NavdataManager()
{
	if(socket != NULL)
	{
		socket->close();
		delete socket;
	}
}

void NavdataManager::init(std::string ip, boost::asio::io_service &io_service)
{
	udp::resolver resolver(io_service);
	udp::resolver::query query(udp::v4(), ip, to_string(ardrone::NAVDATA_PORT));
	udp::endpoint receiver_endpoint = *resolver.resolve(query);
	
	socket = new udp::socket(io_service);
	socket->open(udp::v4());
	socket->bind(udp::endpoint(udp::v4(), ardrone::NAVDATA_PORT));
	
	// Send some data to the navdata port so we start receiving navdata
	
	unsigned char data[4] = {0x01, 0x00, 0x00, 0x00};
	
	socket->send_to(boost::asio::buffer(data), receiver_endpoint);
	
	// Set up navdata reception
	
	startReceive();
}

bool NavdataManager::isConnected()
{
	return navdataAvailable;
}

void NavdataManager::startReceive()
{
	socket->async_receive_from(boost::asio::buffer(_receivedDataBuffer), sender_endpoint, boost::bind(&NavdataManager::packetReceived, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void NavdataManager::packetReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	if(!error)
	{
		parseNavdata(_receivedDataBuffer, bytes_transferred);
	}
	
	// Listen for the next packet
	startReceive();
}

void NavdataManager::update()
{
	socket->get_io_service().poll();
}

AFNavdata *NavdataManager::getNavdata()
{	
	if(isConnected())
	{
		return &navdata;
	}
	else
	{
		return NULL;
	}
}

bool NavdataManager::parseNavdata(char data[], int receivedbytes)
{
	int position = 0;
	
	uint32_t header;
	memcpy(&header, data + position, sizeof(uint32_t));
	position += sizeof(uint32_t);
	
	if(header != 0x55667788) // Magic number :P (Header must always be 0x55667788, as stated in the documentation)
	{
		return false;
	}

	uint32_t dronestate;
	memcpy(&dronestate, data + position, sizeof(uint32_t));
	position += sizeof(uint32_t);
	
	uint32_t seqNum;
	memcpy(&seqNum, data + position, sizeof(uint32_t));
	position += sizeof(uint32_t);

	if(seqNum < last_seqNum)
	{
		return false;
	}
	else
	{
		last_seqNum = seqNum;
	}
	
	uint32_t vision_flag;
	memcpy(&vision_flag, data + position, sizeof(uint32_t));
	position += sizeof(uint32_t);
	
	while(position < receivedbytes)
	{
		uint16_t id, size;
		memcpy(&id, data + position, sizeof(uint16_t));
		position += sizeof(uint16_t);
		memcpy(&size, data + position, sizeof(uint16_t));
		position += sizeof(uint16_t);

		int previous_position = position;

		switch(id)
		{
		case ardrone::navdata_keys::NAVDATA_DEMO_TAG:
			// Read control state
			memcpy(&navdata.ctrlstate, data + position, sizeof(uint32_t));
			position += sizeof(uint32_t);

			// Read battery voltage
			memcpy(&navdata.vbatpercentage, data + position, sizeof(uint32_t));
			position += sizeof(uint32_t);

			// Read euler angles
			memcpy(&navdata.theta, data + position, sizeof(float));
			position += sizeof(float);
			navdata.theta /= 1000.0f;
			
			memcpy(&navdata.phi, data + position, sizeof(float));
			position += sizeof(float);
			navdata.phi /= 1000.0f;

			memcpy(&navdata.psi, data + position, sizeof(float));
			position += sizeof(float);
			navdata.psi /= 1000.0f;

			break;
		case ardrone::navdata_keys::NAVDATA_PHYS_MEASURES_TAG:
			// Ignore first 6 bytes
			position += 6;

			// Read acceleration
			memcpy(&navdata.ax, data + position, sizeof(float));
			position += sizeof(float);
			navdata.ax /= 1000.0f;

			memcpy(&navdata.ay, data + position, sizeof(float));
			position += sizeof(float);
			navdata.ay /= 1000.0f;

			memcpy(&navdata.az, data + position, sizeof(float));
			position += sizeof(float);
			navdata.az /= 1000.0f;
			
			break;
		case ardrone::navdata_keys::NAVDATA_ALTITUDE_TAG:
			// Ignore first 12 bytes
			position += 12;

			int32_t altitude;
			memcpy(&altitude, data + position, sizeof(int32_t));
			position += sizeof(int32_t);
			navdata.altitude = (float) altitude / 1000.0f;

			break;
		case ardrone::navdata_keys::NAVDATA_VISION_RAW_TAG:
			// Read linear velocity
			memcpy(&navdata.vx, data + position, sizeof(float));
			position += sizeof(float);
			navdata.vx /= 1000.0f;

			memcpy(&navdata.vy, data + position, sizeof(float));
			position += sizeof(float);
			navdata.vy /= 1000.0f;

			memcpy(&navdata.vz, data + position, sizeof(float));
			position += sizeof(float);
			navdata.vz /= 1000.0f;

			break;
		}
		
		position += size - (position - previous_position) - 4;
		//                                                  ^ Substract the size of 'id' and 'size'
	}
	
	navdataAvailable = true;
	return true;
}

void NavdataManager::close()
{
	socket->close();
}
