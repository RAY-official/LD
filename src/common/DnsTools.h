#pragma once

#include <string>
#include <vector>

namespace Common {

//class DnsTools
//{
//public:

#ifndef __ANDROID__

  bool fetch_dns_txt(const std::string domain, std::vector<std::string>&records);

#endif

}
