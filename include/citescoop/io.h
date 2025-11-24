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

/// @brief Read protobuf messages from a PBF formatted stream.
class CITESCOOP_EXPORT MessageReader {
 public:
  /// @brief Construct a new message reader.
  /// @param input Input stream to read messages from.
  explicit MessageReader(std::istream* input)
      : zero_copy_stream(input), coded_stream(&zero_copy_stream) {}

  /// @brief Read a message from the input stream.
  /// @tparam T Protobuf message to read. Must inherit from
  /// google::protobuf::Message
  /// @return The parses message.
  template <class T, typename std::enable_if<std::is_base_of<
                         google::protobuf::Message, T>::value>::type* = nullptr>
  std::unique_ptr<T> ReadMessage() {
    uint32_t size;
    coded_stream.ReadRaw(&size, sizeof(size));
    size = ntohl(size);

    auto message = std::make_unique<T>();
    auto limit = coded_stream.PushLimit(static_cast<int>(size));
    message->ParseFromCodedStream(&coded_stream);
    coded_stream.PopLimit(limit);

    return message;
  }

 private:
  google::protobuf::io::IstreamInputStream zero_copy_stream;
  google::protobuf::io::CodedInputStream coded_stream;
};

/// @brief Writer for PBF formatted streams.
///
/// PBF files have the following format:
/// uint32_t Size of next message in network byte order
/// Message
class CITESCOOP_EXPORT MessageWriter {
 public:
  /// @brief Construct a new writer.
  /// @param output Output stream to write messages to.
  explicit MessageWriter(std::ostream* output) : output_stream(output) {}

  /// @brief Write a message to the output stream.
  /// @param message Message to write.
  /// @return Size of message written. Note: this does not include the
  /// size of the uint32_t size written immediately before the
  /// serialized message.
  uint32_t WriteMessage(const google::protobuf::Message& message) {
    uint32_t size = static_cast<uint32_t>(message.ByteSizeLong());
    uint32_t network_size = htonl(size);
    output_stream->write(reinterpret_cast<char*>(&network_size),
                         sizeof(network_size));

    message.SerializeToOstream(output_stream);
    return size;
  }

 private:
  std::ostream* output_stream;
};
}  // namespace wikiopencite::citescoop

#endif  // INCLUDE_CITESCOOP_IO_H_
