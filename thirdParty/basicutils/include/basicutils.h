#include <string>
#include <variant>
#include <iostream>
#include <vector>

using ParamValue = std::variant<
    int, 
    float, 
    bool,
    std::string, 
    std::vector<int>, 
    std::vector<float>,
    std::vector<std::string>
>;
namespace basicutils{
    struct color{
        int red;
        int green;
        int blue;
    };
    struct Param {
        ParamValue value;
    
        template <typename T>
        const T& as() const {
            try {
                return std::get<T>(value);
            } catch (const std::bad_variant_access&) {
                throw std::runtime_error("Param type mismatch");
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