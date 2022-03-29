#include <iostream>
#include <exception>
#include <array>
#include <string>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

using boost::asio::ip::udp; ///??

const unsigned short PORT = 8888;
const std::string PORT_STR{"8888"};
const std::string HOSTNAME{"127.0.0.1"};

const int MAX_BUFFER_SIZE = 1024;

int main(int argc, char *argv[])
{
	boost::asio::io_context io;

	if (argc > 1) // cliente
	{
		try
		{
			udp::resolver resolver{io};
			udp::endpoint destination = *resolver.resolve(udp::v4(), HOSTNAME, PORT_STR).begin();

			udp::socket socket{io};
			socket.open(udp::v4());

			std::array<char, MAX_BUFFER_SIZE> sendData = {"Ping!"};
			std::cout << "Local: Ping!" << std::endl;
			socket.send_to(boost::asio::buffer(sendData), destination);

			std::array<char, 128> recvData;
			udp::endpoint sender;
			size_t len = socket.receive_from(boost::asio::buffer(recvData), sender);

			std::cout.write(recvData.data(), len);
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	else // servidor
	{
		try
		{
			udp::socket socket{io, udp::endpoint{udp::v4(), PORT}};
			std::cout << "A la espera de conexiones..." << std::endl;

			//while(true)
			{
				std::array<char, MAX_BUFFER_SIZE> recvData;
				udp::endpoint endpoint;
				boost::system::error_code error;

				socket.receive_from(boost::asio::buffer(recvData), endpoint, 0, error);
				std::cout.write(recvData.data(), recvData.size());
				std::cout << std::endl;

				if (error){
					throw boost::system::system_error(error);
				}
				std::string message{"Pong!\n"};
				std::cout << "Local: Pong!" << std::endl;

				boost::system::error_code ec;
				socket.send_to(boost::asio::buffer(message), endpoint, 0, ec);
			}
		}
		catch (std::exception &e)
		{
			std::cerr << "Excepcion: " << e.what() << std::endl;
		}
	}
}

// g++ -o pingpong main.cc -pthread -lboost_system -lboost_thread -lboost_date_time && ./pingpong
