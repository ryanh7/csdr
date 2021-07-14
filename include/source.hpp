#pragma once

#include "writer.hpp"

#include <arpa/inet.h>
#include <stdexcept>
#include <thread>

namespace Csdr {

    template <typename T>
    class Source {
        public:
            virtual ~Source() = default;
            virtual void setWriter(Writer<T>* writer);
            virtual Writer<T>* getWriter();
        protected:
            Writer<T>* writer;
    };

    class NetworkException: public std::runtime_error {
        public:
            NetworkException(const std::string& reason): std::runtime_error(reason) {}
    };

    template <typename T>
    class TcpSource: public Source<T> {
        public:
            // TcpSource(std::string remote);
            TcpSource(in_addr_t ip, unsigned short port);
            ~TcpSource();
            void setWriter(Writer<T>* writer) override;
            void stop();
        private:
            void loop();
            int sock;
            bool run = true;
            std::thread* thread = nullptr;
    };

}