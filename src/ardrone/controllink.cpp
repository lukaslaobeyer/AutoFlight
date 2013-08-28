#include "controllink.h"
#include "atcommands/atcommand.h"
#include "atcommands/configcommand.h"
#include "atcommands/configidscommand.h"

using namespace std;
using boost::asio::ip::udp;

ControlLink::ControlLink()
{
	
}

ControlLink::~ControlLink()
{
	if(socket != NULL)
	{
		socket->close();
		delete socket;
	}
}

void ControlLink::init(std::string ip, boost::asio::io_service &io_service)
{
	udp::resolver resolver(io_service);
	udp::resolver::query query(udp::v4(), ip, to_string(ardrone::CONTROL_PORT));
	udp::endpoint receiver_endpoint = *resolver.resolve(query);
	
	socket = new udp::socket(io_service);
	socket->open(udp::v4());
	socket->bind(udp::endpoint(udp::v4(), ardrone::CONTROL_PORT));
	socket->connect(receiver_endpoint);
}

void ControlLink::setAppID()
{
	sendATCommands(vector<ATCommand>{ConfigIDSCommand(), ConfigCommand("custom:session_id", ardrone::app_id::SESSION_ID)});
	sendATCommands(vector<ATCommand>{ConfigIDSCommand(), ConfigCommand("custom:profile_id", ardrone::app_id::PROFILE_ID)});
	sendATCommands(vector<ATCommand>{ConfigIDSCommand(), ConfigCommand("custom:application_id", ardrone::app_id::APPLICATION_ID)});
}

void ControlLink::sendATCommands(vector<ATCommand> cmds)
{
	if(socket)
	{
		stringbuf buf;
		
		const char* AT = "AT*";
		
		for(unsigned int i = 0; i < cmds.size(); i++)
		{
			const char* command = cmds[i].getCommand().c_str();

			string seqNum_str = to_string(seqNum);
			const char* seqNum_c = seqNum_str.c_str();
			
			buf.sputn(AT, 3);
			buf.sputn(command, strlen(command));
			buf.sputc('=');
			buf.sputn(seqNum_c, strlen(seqNum_c));
			
			for(unsigned int j = 0; j < cmds[i].getParameters().size(); j++)
			{
				const char* param = cmds[i].getParameters()[j].c_str();
				
				buf.sputc(',');
				buf.sputn(param, strlen(param));
			}
			
			buf.sputc('\r');
			
			seqNum++;
		}
		
		socket->send(boost::asio::buffer(buf.str()));
	}
	else
	{
		throw ControlLinkNotInitializedException();
	}
}

void ControlLink::close()
{
	socket->close();
}
