#include <string>
#include <sstream>
#include <typeinfo>

#include "exceptions.h"

// строка к объявленному типу
template<typename T>
T toType(std::string str) {
    if constexpr (std::is_same_v<T, std::string>) {
        return str;
    }
    T value;
    if (!(std::stringstream(str) >> value)) {
        throw simple_exception("Wrong field type.");
    }
    return value;
}


// Перегрузка оператора вывода для tuple
template<size_t N, typename T>

struct PrintTuple {
    static void print(std::ostream& out, const T& tuple) {
        PrintTuple<N - 1, T>::print(out, tuple);
        out << (N == 1 ? "" : ", ") << std::get<N - 1>(tuple);
    }
};

template<typename T>
struct PrintTuple<0, T> {
    static void print(std::ostream& os, const T& tuple) {}
};

template <typename... Args>
std::ostream& operator<<(std::ostream& out, const std::tuple<Args...>& tuple) {
    out << "[";
    PrintTuple<sizeof...(Args), std::tuple<Args...>>::print(out, tuple);
    out << "]";
    return out;
}

// Преобразование вектора строк в tuple
template<size_t N, typename T>
struct ToTuple {
    static void convert(T& t, std::vector<std::string>& strV) {
        using elemT = typename std::tuple_element<N - 1, T>::type;
        std::get<N - 1>(t) = toType<elemT>(strV[N - 1]);
        ToTuple<N - 1, T>::convert(t, strV);
    }
};

template<typename T>
struct ToTuple<0, T> {
    static void convert(T&, std::vector<std::string>&) {}
};

template<typename... Args>
std::tuple<Args...> toTuple(std::vector<std::string>& strV) {
    std::tuple<Args...> tuple;
    ToTuple<sizeof...(Args), std::tuple<Args...>>::convert(tuple, strV);
    return tuple;
}