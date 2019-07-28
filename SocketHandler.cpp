#include "SocketHandler.h"

#include <assert.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "CallbackHandler.h"

using namespace boost::asio::ip;

SocketWrapper::~SocketWrapper() {
	switch (socketType) {
		case SM_SocketType_Tcp:
			delete (Socket<tcp>*) socket;
			break;
		case SM_SocketType_Udp:
			delete (Socket<udp>*) socket;
			break;
#if 0
		case SM_SocketType_Icmp:
			delete (Socket<icmp>*) socket;
			break;
#endif
	}

	callbackHandler.RemoveCallbacks(this);
}

template Socket<tcp>* SocketHandler::CreateSocket<tcp>(SM_SocketType);
template Socket<udp>* SocketHandler::CreateSocket<udp>(SM_SocketType);

SocketHandler::SocketHandler() : ioWorkGuard(io.get_executor()) {
}

SocketHandler::~SocketHandler() {
	if (!socketList.empty() || ioServiceProcessingThread) {
		Shutdown();
	}
}

void SocketHandler::Shutdown() {
	boost::mutex::scoped_lock l(socketListMutex);

	for (auto it=socketList.begin(); it!=socketList.end(); it++) {
		delete *it;
	}

	socketList.clear();

	if (ioServiceProcessingThread) StopProcessing();
}

template <class SocketType>
Socket<SocketType>* SocketHandler::CreateSocket(SM_SocketType st) {
	boost::mutex::scoped_lock l(socketListMutex);

	SocketWrapper* sp = new SocketWrapper(new Socket<SocketType>(st), st);
	socketList.push_back(sp);

	return (Socket<SocketType>*) sp->socket;
}

void SocketHandler::DestroySocket(SocketWrapper* sw) {
	assert(sw);

	{ // lock
		boost::mutex::scoped_lock l(socketListMutex);

		for (auto it=socketList.begin(); it!=socketList.end(); it++) {
			if (*it == sw) {
				socketList.erase(it);
				break;
			}
		}
	} // ~lock

	delete sw;
}

void SocketHandler::StartProcessing() {
	assert(!ioServiceProcessingThread);

	ioServiceProcessingThread = std::make_unique<boost::thread>(boost::bind(&SocketHandler::RunIoService, this));
}

void SocketHandler::StopProcessing() {
	assert(ioServiceProcessingThread);

	io.stop();
	ioServiceProcessingThread->join();
	ioServiceProcessingThread.reset();
}

void SocketHandler::RunIoService() {
	io.run();
}

SocketWrapper* SocketHandler::GetSocketWrapper(const void* socket) {
	boost::mutex::scoped_lock l(socketListMutex);

	for (auto it=socketList.begin(); it!=socketList.end(); it++) {
		if ((*it)->socket == socket) return *it;
	}

	return NULL;
}

SocketHandler socketHandler;

