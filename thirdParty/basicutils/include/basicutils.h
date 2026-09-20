#include <iostream>
#include <stdexcept>
#include <source_location>
#include <format>
#include <string>
#include <variant>
#include <vector>

namespace basicutils{
    using ParamValue = std::variant<
        int, 
        float, 
        double, 
        bool,
        std::string, 
        std::vector<int>, 
        std::vector<float>,
        std::vector<double>,
        std::vector<std::string>
    >;
    struct color{
        int red;
        int green;
        int blue;
    };
    struct Param {
        ParamValue value;
    
        template <typename T>
        const T& as(const std::source_location location = std::source_location::current()) const {
            if(std::holds_alternative<T>(value)){
                return std::get<T>(value);
            }
            else {
                std::string msg = std::format("[{}][{}][{}] Param type mismatch", location.file_name(), location.function_name(), location.line());
                throw std::runtime_error(msg);
            }
        }
    
        template <typename T>
        bool is() const {
            return std::holds_alternative<T>(value);
        }
    };
    int getRGBChannelFromHex(std::string hex, std::string rgbChannel);
    void getRGBColorFromHex(color currentColor, std::string hex);


}