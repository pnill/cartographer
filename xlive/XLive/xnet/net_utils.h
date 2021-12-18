#pragma once

std::string IOCTLSocket_cmd_string(long cmd);
std::string sockOpt_string(int optName);

bool Ipv4AddressIsReservedOrLocalhost(const IN_ADDR ipv4Addr);