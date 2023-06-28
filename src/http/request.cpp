#include "./request.hpp"
#include <algorithm>
#include <utility>

// Request
http::Request::Request(int client_fd, size_t req_size_limit)
{
    this->client_fd = client_fd;
    this->buf_limit = req_size_limit;
    this->buffer.resize(this->buf_limit);


    this->bytes_recv = recv(this->client_fd, this->buffer.data(), this->buf_limit, this->recv_flag);
    this->buffer.assign(buffer, this->bytes_recv);  // Adjust the buffer size based on the bytes received

    switch (this->from_buffer()) {
    case 0:
        break;
    case -1:
        std::cerr << "Buffer empty" << std::endl;
        break;
    }
}

size_t http::Request::parse_routing() noexcept
{
    auto header = this->buffer.substr(0, this->buffer.find("\r\n"));
    // Parse header line

    int spaces = 0;
    std::string buffer;
    for (const char& ch : header) {
        if (ch == ' ') {
            switch (spaces) {
            case 0:
                this->method = buffer;
                break;
            case 1:
                this->route = buffer;
                break;
            }
            spaces++;
            buffer.clear();
        } else {
            buffer.push_back(ch);
        }
    }
    this->http_version = buffer;
    return this->buffer.find("\r\n");
}

std::vector<std::pair<std::string, std::string>> extract_headers(const std::string& buff)
{
    std::vector<std::string> lines;
    std::vector<std::pair<std::string, std::string>> headers;

    size_t start = 0;
    size_t newline;
    while ((newline = buff.find("\r\n", start)) != std::string::npos) {
        lines.push_back(buff.substr(start, newline - start));
        start = newline + 2;  // skip over the "\r\n" delimiter
    }
    // add the last line, if any
    if (start < buff.size()) {
        lines.push_back(buff.substr(start));
    }

    for (const auto& line : lines) {
        size_t delim = line.find(':');
        if (delim != std::string::npos) {
            size_t delim = line.find(':');
            std::string key, val;
            key = line.substr(0, delim);
            val = line.substr(delim + 1, std::string::npos);
            std::pair<std::string, std::string> header(key, val);
            headers.push_back(header);
        }
    }

    return headers;
}

int http::Request::from_buffer()
{
    if (this->buffer.empty()) {
        return -1;
    }

    size_t end_first_line = this->parse_routing();
    // std::cout << "DEBUG: " << end_first_line << std::endl;
    std::vector<std::pair<std::string, std::string>> headers;
    auto body_delim = this->buffer.find("\r\n\r\n");
    // std::cout << "DEBUG: " << body_delim << std::endl;
    if (body_delim != std::string::npos) {
        this->body = this->buffer.substr(body_delim + std::strlen("\r\n\r\n"), std::string::npos);
        auto header_buff = this->buffer.substr(end_first_line, body_delim);
        headers = extract_headers(header_buff);
    } else {
        auto header_buff = this->buffer.substr(end_first_line, std::string::npos);
        headers = extract_headers(header_buff);
    }

    if (!headers.empty()) {
        for (const auto& pair : headers) {
            auto save_result = this->headers.insert_or_assign(pair.first, pair.second);
            if (!save_result.second) {
                std::cerr << "\033[1;31m"
                          << "Error: "
                          << "\033[0m"
                          << "Could not save header: " << pair.first << std::endl;
            }
        }
    }

    return 0;
}

const std::string http::Request::to_string()
{
    std::string out;
    out.append(this->method);
    out.append(this->route);
    out.append(this->http_version);
    out.append("\r\n");
    for (const auto& pair : this->headers) {
        out.append(pair.first);
        out.append(": ");
        out.append(pair.second);
        out.append("\r\n");
    }
    out.append("\r\n\r\n");
    out.append(this->body);
    return out;
}
