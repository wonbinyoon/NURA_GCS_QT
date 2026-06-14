#ifndef IPROTOCOLPARSER_H
#define IPROTOCOLPARSER_H

#include <vector>
#include <cstdint>
#include "../model/DataFrame.h"

class IProtocolParser {
public:
    virtual ~IProtocolParser() = default;

    // Parse an incoming stream of bytes and return a vector of extracted DataFrames
    virtual std::vector<DataFrame> parse(const std::vector<uint8_t>& data) = 0;
};

#endif // IPROTOCOLPARSER_H
