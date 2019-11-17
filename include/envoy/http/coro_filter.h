#pragma once

#include <experimental/coroutines>

#include "envoy/http/filter.h"

namespace Envoy {
namespace Http {

class StreamDecoderCoroFilter : public StreamDecoderFilter {
public:
  bool is_end_stream() const { return end_stream_; }

  template<class Result, StreamDecoderCoroFilter::*Result* result>
  class Awaiter {
  public:
    Awaiter() {}
    Awaiter(StreamDecoderCoroFilter* filter, State expected) : filter_(filter), state_(state) {}
    bool await_ready() { return !filter_; }
    void await_suspend(std::experimental::coroutine_handle<> h) {
      filter_->coro_ = h;
    }
    Result* await_resume() {
      if (filter_ && filter_->state_ == expected_) {
        return filter_->*result;
      }
      return nullptr;
    }
  private:
    StreamDecoderCoroFilter* filter_{};
    State expected_{};
  };

  Awaiter<HeaderMap, &StreamDecoderCoroFilter::headers_> decodeHeaders() {
    if (end_stream_) {
      return {};
    }
    return {this, State::Headers};
  }

  Awaiter<Buffer::Instance, &StreamDecoderCoroFilter::headers_> decodeData() {
    if (end_stream_) {
      return {};
    }
    return {this, State::Data};
  }

  Awaiter<HeaderMap, &StreamDecoderCoroFilter::trailers_> decodeTrailers() {
    if (end_stream_) {
      return {};
    }
    return {this, State::Trailers};
  }

protected:
  FilterHeadersStatus decodeHeaders(HeaderMap& headers, bool end_stream) override {
    state_ = State::Headers;
    headers_ = &headers;
    end_stream_ = end_stream;
    coro_.resume();
    return FilterHeadersStatus::Continue;
  }
  FilterDataStatus decodeData(Buffer::Instance& data, bool end_stream) override {
    state_ = State::Data;
    data_ = data;
    end_stream_ = end_stream;
    coro_.resume();
    return FilterDataStatus::Continue;
  }
  FilterTrailersStatus decodeTrailers(HeaderMap& trailers) override {
    state_ = State::Trailers;
    trailers_ = trailers;
    coro_.resume();
    return FilterTrailersStatus::Continue;
  }

private:
  enum class State {
    None, Headers, Data, Trailers,
  };
  State state_{State::None};
  std::experimental::coroutine_handle<> coro_;
  HeaderMap* headers_{};
  Buffer::Instance* data_{};
  HeaderMap* trailers_;
  bool end_stream_{false};
};

} // namespace Http
} // namespace Envoy
