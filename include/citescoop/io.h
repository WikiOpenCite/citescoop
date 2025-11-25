// SPDX-FileCopyrightText: 2025 The University of St Andrews
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INCLUDE_CITESCOOP_IO_H_
#define INCLUDE_CITESCOOP_IO_H_

// NOLINTNEXTLINE(misc-include-cleaner)
#include <arpa/inet.h>

#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <type_traits>

#include "google/protobuf/io/coded_stream.h"
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
      : zero_copy_stream_(input), coded_stream_(&zero_copy_stream_) {}

  /// @brief Read a message from the input stream.
  /// @tparam T Protobuf message to read. Must inherit from
  /// google::protobuf::Message
  /// @return The parses message.
  template <class T>
  requires std::is_base_of_v<google::protobuf::Message, T> std::unique_ptr<T>
  ReadMessage() {
    uint32_t size;
    coded_stream_.ReadRaw(&size, sizeof(size));

    // NOLINTNEXTLINE(misc-include-cleaner)
    size = ntohl(size);

    auto message = std::make_unique<T>();
    auto limit = coded_stream_.PushLimit(static_cast<int>(size));
    message->ParseFromCodedStream(&coded_stream_);
    coded_stream_.PopLimit(limit);

    return message;
  }

 private:
  google::protobuf::io::IstreamInputStream zero_copy_stream_;
  google::protobuf::io::CodedInputStream coded_stream_;
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
  explicit MessageWriter(std::ostream* output) : output_stream_(output) {}

  /// @brief Write a message to the output stream.
  /// @param message Message to write.
  /// @return Size of message written. Note: this does not include the
  /// size of the uint32_t size written immediately before the
  /// serialized message.
  uint32_t WriteMessage(const google::protobuf::Message& message) {
    // Value changes dependent upon parameters of method call.
    // NOLINTNEXTLINE(readability-identifier-naming)
    const auto size = static_cast<uint32_t>(message.ByteSizeLong());

    // NOLINTNEXTLINE(misc-include-cleaner)
    uint32_t network_size = htonl(size);
    output_stream_->write(reinterpret_cast<char*>(&network_size),
                          sizeof(network_size));

    message.SerializeToOstream(output_stream_);
    return size;
  }

 private:
  std::ostream* output_stream_;
};
}  // namespace wikiopencite::citescoop

#endif  // INCLUDE_CITESCOOP_IO_H_
