#ifndef INC_SEXT_SOCKETHANDLER_H
#define INC_SEXT_SOCKETHANDLER_H

#include <deque>
#include <memory>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "Socket.h"
#include "Define.h"

struct SocketWrapper {
	SocketWrapper(void* socket, SM_SocketType socketType) noexcept : socket(socket), socketType(socketType) {}
	~SocketWrapper();

	void* socket;
	SM_SocketType socketType;
};

class SocketHandler {
public:
	SocketHandler();
	~SocketHandler();

	void Shutdown();

	SocketWrapper* GetSocketWrapper(const void* socket);

	template <class SocketType> Socket<SocketType>* CreateSocket(SM_SocketType st);
	void DestroySocket(SocketWrapper* sw);

	void StartProcessing();
	void StopProcessing();

	//friend class Socket;
	boost::asio::io_context io;

private:
	std::deque<SocketWrapper*> socketList;
	boost::mutex socketListMutex;

	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> ioWorkGuard;

	std::unique_ptr<boost::thread> ioServiceProcessingThread;

	void RunIoService();
};

extern SocketHandler socketHandler;

#endif

