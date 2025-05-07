#pragma once

#include <functional>

namespace{
    class Command{
        private :
        std::function<void()> func = [](){};
        
        public :
        Command(){};
        Command(std::function<void()> f):func(f){};
        void operator()(){func();};
    };
} // namespace