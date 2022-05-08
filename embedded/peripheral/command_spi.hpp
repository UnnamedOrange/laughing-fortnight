/**
 * @file command_spi.hpp
 * @author UnnamedOrange
 * @brief 使用 SPI 发送和接收指令。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <cinttypes>
#include <string>
#include <string_view>
#include <type_traits>

#include "command_receiver_base.hpp"
#include "command_sender_base.hpp"

namespace peripheral
{
    /**
     * @brief 使用 SPI 发送和接收指令。
     *
     * @note 这个类是线程安全的。
     *
     * @todo 确认位宽应该选取为 8。
     *
     * @tparam frequency SPI 的频率。默认为 1 MHz。
     * @tparam bits SPI 的位宽。默认为 8。
     * @tparam mode SPI 的工作模式。默认为 3。
     */
    template <int frequency = 1000000, int bits = 8, int mode = 3>
    class command_spi
    {
    private:
        mbed::SPI& _spi;
        mbed::DigitalOut& _cs;

    public:
        command_spi(mbed::SPI& spi, mbed::DigitalOut& cs) : _spi(spi), _cs(cs)
        {
            _cs = 1;
            _spi.format(bits, mode);
            _spi.frequency(frequency);
        }

    private:
        void _select()
        {
            _spi.lock();
            _cs = 0;
        }
        void _deselect()
        {
            _cs = 1;
            _spi.unlock();
        }

    public:
        /**
         * @brief 使用 SPI 发送并接收一个数据。数据的宽度取决于 bits。
         *
         * @note 这个函数是线程安全的。
         *
         * @param data 要发送的数据。
         * @return int 接收到的数据。
         */
        int write(int data)
        {
            static_assert(sizeof(int) * 8 >= bits, "int is not large enough.");
            _select();
            int ret = _spi.write(data);
            _deselect();
            return ret;
        }
        /**
         * @brief 使用 SPI 阻塞地发送并接收一个字节流。
         *
         * @note 这个函数是线程安全的。
         *
         * @todo 确认得到的数据是期望的。
         *
         * @param data 要发送的字节流。
         * @return std::string 接收到的字节流。长度与发送的字节流相等。
         */
        std::string write(std::string_view data)
        {
            // 注意 std::string 对 0 的处理。
            std::string buffer(data.size(), '\0');
            _select();
            auto n_succeeded =
                _spi.write(data.data(), static_cast<int>(data.size()),
                           const_cast<char*>(buffer.data()),
                           static_cast<int>(buffer.size()));
            _deselect();
            assert(n_succeeded == data.size());
            return buffer;
        }
        /**
         * @brief 使用 SPI 阻塞地发送并接收一个字节流。
         *
         * @note 这个函数是线程安全的。
         *
         * @param native_array 要发送的字节流。
         * @return std::string 接收到的字节流。长度与发送的字节流相等。
         */
        template <size_t size>
        std::string write(const char (&native_array)[size])
        {
            return write(std::string_view(native_array, size));
        }
        /**
         * @brief 使用 SPI 阻塞地发送并接收一个字节流。
         *
         * @note 这个函数是线程安全的。
         *
         * @param array_object 要发送的字节流。
         * @return std::string 接收到的字节流。长度与发送的字节流相等。
         */
        template <typename array_like_t>
        std::string write(const array_like_t& array_object)
        {
            static_assert(
                std::is_same<typename array_like_t::value_type, char>::value,
                "array_like_t must be an array of char.");
            return write(
                std::string_view(array_object.data(), array_object.size()));
        }
    };
} // namespace peripheral
