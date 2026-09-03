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
    struct loopValues{
        int n{0};
    };
}