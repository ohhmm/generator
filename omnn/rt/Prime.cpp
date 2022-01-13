#include "Prime.h"
#include <cassert>
#include <thread>
#include <utility>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/multiprecision/miller_rabin.hpp>

#include <rt/tasq.h>

#define PRIME_LIST_FILENAME "Primes.inc"
static const boost::multiprecision::cpp_int Primes[] = {
#include "Primes.inc"
};
static const size_t PrimeItems = sizeof(Primes) / sizeof(Primes[0]);

namespace {
#ifdef rt_SRC_DIR
boost::filesystem::path PrimeListDir = rt_SRC_DIR;
auto PrimeListPath = PrimeListDir / PRIME_LIST_FILENAME;
#endif
} // namespace

namespace omnn::rt {

bool GrowPrime(const boost::multiprecision::cpp_int& upto,
               std::function<bool(boost::multiprecision::cpp_int)> is_prime) {
    auto& prev = Primes[PrimeItems - 1];
    auto next = prev;
    ++next;
    static auto from = next;
    auto range = upto;
    range -= prev;
    auto chunks = std::thread::hardware_concurrency() * 2; //TODO:
    auto chunk = range;
    range /= chunks;
    std::deque<std::future<std::string>> primining;
    for (decltype(chunks) i = 0; i <= chunks; ++i) {
        primining.emplace_back(std::async(std::launch::async, [=]() {
            std::stringstream ss;
            auto j = chunk;
            j *= i;
            j += from;
            auto up = j + chunk;
            for (; j < up; ++j) {
                if (is_prime(j))
                    ss << j << ',';
            }
            return std::move(ss.str());
        }));
    }

    while (primining.size()) {
        auto line = primining.front().get();
        std::ofstream PrimesIncFile(PrimeListPath.string(), std::ios_base::app);
        PrimesIncFile << std::endl << line;
        PrimesIncFile.close();
        primining.pop_front();
    }
    from++ = upto;
    return true;
}

const boost::multiprecision::cpp_int& prime(size_t idx) {
#ifndef NDEBUG
    auto haveThePrime = idx < PrimeItems;
    if (!haveThePrime) {
        std::cerr << "There is no " << idx
                  << "th prime in the table yet. Use GrowPrime call if you use debug version to increase the table for "
                     "next compile time. Currently the table has "
                  << PrimeItems << " elements" << std::endl;
        assert(haveThePrime);
    }
#endif
    return Primes[idx];
}

size_t primes() { return PrimeItems - 1; }

} // namespace omnn::rt