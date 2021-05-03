#include "EventHandler.hpp"
std::map<EventType, std::vector<EventCallback<void*>>> event_map;