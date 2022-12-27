#include <exception>

class simple_exception : public std::exception {
private:
    std::string msg{};
public:
    explicit simple_exception(std::string const& message) : msg(message) {}
    char const *what() const noexcept override {
        return msg.c_str();
    }
};