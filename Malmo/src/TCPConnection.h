// --------------------------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft Corporation.  All rights reserved.
// --------------------------------------------------------------------------------------------------------------------

#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_

// Local:
#include "TimestampedUnsignedCharVector.h"

// Boost:
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

// STL:
#include <string>

namespace malmo
{
    //! Handles an individual TCP connection.
    class TCPConnection : public boost::enable_shared_from_this< TCPConnection >
    {
        public:

            static boost::shared_ptr<TCPConnection> create(
                boost::asio::io_service& io_service, 
                boost::function<void(const TimestampedUnsignedCharVector message) > callback,
                bool expect_size_header );

            boost::asio::ip::tcp::socket& getSocket();

            void read();
            
            void confirmWithFixedReply(std::string reply);

        private:

            TCPConnection(boost::asio::io_service& io_service, boost::function<void(const TimestampedUnsignedCharVector) > callback, bool expect_size_header);

            // called when we have successfully read the size header
            void handle_read_header( const boost::system::error_code& error, size_t bytes_transferred );

            // called when we have successfully read the body of the message, of known length
            void handle_read_body( const boost::system::error_code& error, size_t bytes_transferred );

            // called when we have successfully read the body of the message terminated by a newline
            void handle_read_line( const boost::system::error_code& error, size_t bytes_transferred );

            size_t getSizeFromHeader();
            void processMessage();
            void sendReply();

        private:

            boost::asio::ip::tcp::socket socket;

            static const int SIZE_HEADER_LENGTH = 4;
            boost::asio::streambuf delimited_buffer;
            std::vector<unsigned char> header_buffer;
            std::vector<unsigned char> body_buffer;

            boost::function<void(const TimestampedUnsignedCharVector message) > onMessageReceived;
            
            bool confirm_with_fixed_reply;
            std::string fixed_reply;
            bool expect_size_header;
    };
}

#endif
