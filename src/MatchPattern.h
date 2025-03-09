#ifndef MATCHPATTERN_H
#define MATCHPATTERN_H

#include <boost/asio.hpp>
#include <boost/asio/buffers_iterator.hpp>

#include <vector>
#include <cstdint>

namespace uart
{
    // make things more readable
    using BuffersIterator = boost::asio::buffers_iterator<boost::asio::const_buffer>;
    using MatchResult = std::pair<BuffersIterator, bool>;

    /**
     * @brief Class to match a pattern in a stream of bytes
     *
     * This class is used to match a pattern in a stream of bytes, meant for use with the
     * boost::asio::async_read_until function.
     */
    class MatchPattern
    {
    public:
        explicit MatchPattern(const std::vector<uint8_t> &pattern) : m_pattern(pattern) {}

        // holy hell, asio WILL NOT COOPERATE WITHOUT TEMPLATING THIS FUNCTION
        // defining opertaor() means this is an function object
        // this is the function that will be called by asio to match the pattern
        template <typename Iterator>
        std::pair<Iterator, bool> operator()(Iterator begin, Iterator end) const
        {
            auto iterator = begin;

            while (iterator != end)
            {
                if (std::distance(iterator, end) >= static_cast<int>(m_pattern.size()) && // do we have enough data to compare
                    std::equal(m_pattern.begin(), m_pattern.end(), iterator))             // comparison happens here
                {
                    return std::make_pair(std::next(iterator, m_pattern.size()), true);
                }
                ++iterator;
            }
            return std::make_pair(iterator, false);
        }

    private:
        const std::vector<uint8_t> m_pattern;
    };
}

// Tell Boost.Asio that MatchPattern is a valid match condition
// this is required for successfull compilation
namespace boost
{
    namespace asio
    {
        template <>
        struct is_match_condition<uart::MatchPattern> : std::true_type
        {
        };
    }
}

#endif // MATCHPATTERN_H