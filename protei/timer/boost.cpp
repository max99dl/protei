#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
int main()
{
    //boost::posix_time::ptime datetime = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::ptime datetime;
 		std::string data = to_simple_string(datetime);
    std::cout << "Current Time and Date: " << data << std::endl;
 
    return 0;
}
