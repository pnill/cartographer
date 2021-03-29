#pragma once

std::string IOCTLSocket_cmd_string(long cmd);

bool Ipv4AddressIsReservedOrLocalhost(const IN_ADDR ipv4Addr);