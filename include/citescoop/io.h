// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INCLUDE_CITESCOOP_IO_H_
#define INCLUDE_CITESCOOP_IO_H_

#include <arpa/inet.h>

#include <istream>
#include <memory>
#include <ostream>

#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/message.h"

#include "citescoop/citescoop_export.h"

namespace wikiopencite::citescoop {

class CITESCOOP_EXPORT MessageReader {
 public:
  explicit MessageReader(std::shared_ptr<std::istream> input) {
    zero_copy_stream =
        std::make_shared<google::protobuf::io::IstreamInputStream>(input.get());
    coded_stream = std::make_unique<google::protobuf::io::CodedInputStream>(
        zero_copy_stream.get());
  }

  template <class T, typename std::enable_if<std::is_base_of<
                         google::protobuf::Message, T>::value>::type* = nullptr>
  std::unique_ptr<T> ReadMessage() {
    uint32_t size;
    coded_stream->ReadRaw(&size, sizeof(size));
    size = ntohl(size);

    auto message = std::make_unique<T>();
    auto limit = coded_stream->PushLimit(static_cast<int>(size));
    message->ParseFromCodedStream(coded_stream.get());
    coded_stream->PopLimit(limit);

    return message;
  }

 private:
  std::shared_ptr<google::protobuf::io::ZeroCopyInputStream> zero_copy_stream;
  std::unique_ptr<google::protobuf::io::CodedInputStream> coded_stream;
};

class CITESCOOP_EXPORT MessageWriter {
 public:
  explicit MessageWriter(std::shared_ptr<std::ostream> output) {
    output_stream = output;
  }

  uint32_t WriteMessage(const google::protobuf::Message& message) {
    uint32_t size = static_cast<uint32_t>(message.ByteSizeLong());
    uint32_t network_size = htonl(size);
    output_stream->write(reinterpret_cast<char*>(&network_size),
                         sizeof(network_size));

    message.SerializeToOstream(output_stream.get());
    return size;
  }

 private:
  std::shared_ptr<std::ostream> output_stream;
};
}  // namespace wikiopencite::citescoop

#endif  // INCLUDE_CITESCOOP_IO_H_
