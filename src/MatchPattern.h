#ifndef MATCHPATTERN_H
#define MATCHPATTERN_H

#include <boost/asio.hpp>
#include <boost/asio/buffers_iterator.hpp>

#include <vector>
#include <cstdint>

namespace uart
{
    using BuffersIterator = boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type>;
    using MatchResult = std::pair<BuffersIterator, bool>;

    class MatchPattern
    {

    public:
        explicit MatchPattern(const std::vector<uint8_t> &pattern) : m_pattern(pattern) {}

        MatchResult operator()(BuffersIterator begin, BuffersIterator end)
        {

            auto iterator = begin;
            while (iterator != end)
            {
                if (std::distance(iterator, end) >= static_cast<int>(m_pattern.size()) &&
                    std::equal(m_pattern.begin(), m_pattern.end(), iterator))
                {
                    return {std::next(iterator, m_pattern.size()), true};
                }
                ++iterator;
            }
            return {iterator, false};
        }

    private:
        std::vector<uint8_t>
            m_pattern;
    };
}

#endif // MATCHPATTERN_H