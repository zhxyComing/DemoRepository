//
// Created by 徐政 on 9/23/21.
//

#ifndef GLREVIEW_LOG_H
#define GLREVIEW_LOG_H

#include <iostream>

// 可以只有头文件，如果报错了，可能是CMake缓存了之前对应的cpp、后来cpp删掉导致的。

class Log {
private:
    Log() = default;

public:
    template<typename M>
    static void i(const char *, const M *);

    template<typename M>
    static void i(const char *, M);

    template<typename M>
    static void e(M);
};

template<typename M>
void Log::i(const char *tag, const M *msg) {
    std::cout << tag << ": " << msg << std::endl;
}

template<typename M>
void Log::i(const char *tag, M msg) {
    std::cout << tag << ": " << msg << std::endl;
}

template<typename M>
void Log::e(M msg) {
    std::cout << "ERROR" << ": " << msg << std::endl;
}


#endif //GLREVIEW_LOG_H
