#ifdef __cplusplus

#ifndef __ARDUINO_RPC_IMPLEMENTATION__
#define __ARDUINO_RPC_IMPLEMENTATION__

// msgpack overrides BIN symbol, undefine it 
#ifdef BIN
#define _BIN BIN
#undef BIN
#define BIN BIN_MSGPACK
#endif
#include "rpclib.h"
#include "rpc/dispatcher.h"
#include "RPC_client.h"
#ifdef _BIN
#undef BIN
#define BIN _BIN
#endif

extern "C" {
#ifdef ATOMIC_FLAG_INIT
#undef ATOMIC_FLAG_INIT
#endif
#ifdef ATOMIC_VAR_INIT
#undef ATOMIC_VAR_INIT
#endif
#define boolean   boolean_t
#include "openamp.h"
#include "arduino_openamp.h"
#undef boolean
#ifdef bool
#define boolean   bool
#endif
}

#include "mbed.h"

typedef struct _service_request {
  uint8_t* data;
} service_request;

namespace arduino {

class RPCClass : public Stream, public rpc::detail::dispatcher {
	public:
		RPCClass() {};
		int begin();
		void end() {};
		int available(void) {
			return rx_buffer.available();
		};
		int peek(void) {
			return rx_buffer.peek();
		}
		int read(void) {
			return rx_buffer.read_char();
		}
		void flush(void) {};
		size_t write(uint8_t c);
		size_t write(const uint8_t*, size_t);
		size_t write(uint8_t ep, const uint8_t* buf, size_t len);

		using Print::write; // pull in write(str) and write(buf, size) from Print
		operator bool() {
			return initialized;
		}

	    void attach(void (*fptr)(void))
	    {
	        if (fptr != NULL) {
	            _rx = mbed::Callback<void()>(fptr);
	        }
	    }

		template <typename... Args>
    	RPCLIB_MSGPACK::object_handle call(std::string const &func_name,
                                       Args... args) {
    		// find a free spot in clients[]
    		// create new object
    		// protect this with mutex

    		mtx.lock();
    		int i = 0;
    		for (i=0; i<10; i++) {
    			if (clients[i] == NULL) {
    				clients[i] = new rpc::client();
    				break;
    			}
    		}
    		mtx.unlock();

    		// thread start and client .call
    		clients[i]->call(func_name, args...);
    		RPCLIB_MSGPACK::object_handle ret = std::move(clients[i]->result);

    		mtx.lock();
    		delete clients[i];
    		clients[i] = NULL;
    		mtx.unlock();
    		return ret;
    	}

		rpc::client* clients[10];

	private:
		RingBufferN<256> rx_buffer;
		bool initialized = false;

		static int rpmsg_recv_callback(struct rpmsg_endpoint *ept, void *data,
                                       size_t len, uint32_t src, void *priv);
		static int rpmsg_recv_response_callback(struct rpmsg_endpoint *ept, void *data,
                                       size_t len, uint32_t src, void *priv);

		static void new_service_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest);

		void dispatch();
		void response();
		events::EventQueue eventQueue;
		mbed::Ticker ticker;
		rtos::Thread* eventThread;
		rtos::Thread* dispatcherThread;
		rtos::Thread* responseThread;
		rtos::Mutex mtx;

		mbed::Callback<void()> _rx;

		//rpc::detail::response response;
		RPCLIB_MSGPACK::object_handle call_result;

		osThreadId dispatcherThreadId;
		osThreadId responseThreadId;
};
}

extern arduino::RPCClass RPC;

#endif
#endif
