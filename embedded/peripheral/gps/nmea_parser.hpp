/**
 * @file nmea_parser.hpp
 * @author UnnamedOrange
 * @brief 接收 NMEA 数据并解析。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

#include "../command_receiver_serial.hpp"
#include "../peripheral_thread.hpp"
#include <utils/debug.hpp>

namespace peripheral
{
    /**
     * @brief 接收 NMEA 数据并解析。内部会创建一个线程不断读串口。
     *
     * @note 接收到新的位置后不会进行通知。
     */
    class nmea_parser : public peripheral_thread
    {
    private:
        command_receiver_serial& _receiver;

    private:
        bool _should_exit{};

    public:
        nmea_parser(command_receiver_serial& receiver) : _receiver(receiver)
        {
            peripheral_thread::start();
        }
        ~nmea_parser()
        {
            _should_exit = true;
            peripheral_thread::join();
        }

    private:
        /**
         * @brief 不断读取串口信息的线程函数。
         */
        void thread_main() override
        {
            std::string buf;
            while (true)
            {
                using namespace std::literals;
                if (_should_exit)
                    break;
                // 非阻塞地读取串口，以保证线程可正常退出。
                std::string read_str = _receiver.receive_command(10ms);
                for (char ch : read_str)
                {
                    buf.push_back(ch);
                    if (buf.back() == '\n')
                    {
                        // 弹出所有的换行。
                        while (!buf.empty() && buf.back() == '\n')
                            buf.pop_back();
                        while (!buf.empty() && buf.back() == '\r')
                            buf.pop_back();
                        // 如果不是空行，则处理。
                        if (!buf.empty())
                            parse_frame(buf);
                        // 处理完成，清空缓冲区。
                        buf.clear();
                    }
                }
            }
        }

    private:
        /**
         * @brief 将 NMEA 帧以逗号为分隔符、星号为结尾符分割，并验证校验值。
         *
         * @param frame 不包含换行符的一个 NMEA 帧。
         * @return std::vector<std::string>
         * 如果校验成功，则返回各字符串组成的数组。 否则返回一个空数组。
         */
        static std::vector<std::string> split_frame_and_verify(
            std::string_view frame)
        {
            std::vector<std::string> ret;
            // 假设第一个字符是 $。
            if (frame[0] != '$') // 如果不是 NMEA 帧，则失败。
                return ret;      // 返回空数组。
            // 假设最后三个字符是 *XX，校验。
            if (frame.length() < 3) // 数组长度小于 3 则失败。
                return ret;         // 返回空数组。
            if (frame[frame.length() - 3] != '*') // 不是星号结尾，则失败。
                return ret;                       // 返回空数组。
            {
                int desired_check_sum;
                std::string last_two{frame.substr(frame.length() - 2)};
                if (1 != sscanf(last_two.c_str(), "%x", &desired_check_sum))
                    return ret; // 解析失败，返回空数组。

                // 取中间部分。经过前面的检查，长度总是符合要求。
                std::string_view middle = frame.substr(1, frame.length() - 4);
                int check_sum = std::accumulate(middle.begin(), middle.end(),
                                                int{}, std::bit_xor<int>{});
                if (desired_check_sum != check_sum) // 校验和错误，则失败。
                    return ret;                     // 返回空数组。
            }

            // 将 frame 以逗号为分隔符，星号为结尾符切割。
            std::string buf;
            for (char ch : frame)
            {
                if (ch == ',' || ch == '*')
                {
                    ret.push_back(std::move(buf));
                    buf.clear(); // 假设 move 后的行为是不确定的。
                    if (ch == '*')
                        break;
                }
                else
                    buf.push_back(ch);
            }

            return ret;
        }
        /**
         * @brief 解析 NMEA 帧。
         *
         * @param frame 原始帧字符串，不包含结尾的换行符。
         */
        void parse_frame(std::string_view frame)
        {
            auto parts = split_frame_and_verify(frame);
            if (parts.empty()) // 为空说明解析失败，不处理。
                return;

            if (parts[0] == "$GPRMC") // 推荐的定位信息。
                parse_gprmc(parts);
            else // 未知的地址域，不处理。
                return;
        }

    public:
        /**
         * @brief 位置信息类型。
         */
        struct position_t
        {
            /**
             * @brief 位置信息是否有效。为真时其他参数才可用。
             */
            bool is_valid;
            /**
             * @brief 纬度。度分格式。
             */
            std::string latitude;
            /**
             * @brief 纬度半球。"N"表示北半球，"S"表示南半球。
             */
            std::string latitude_semi;
            /**
             * @brief 精度。度分格式。
             */
            std::string longitude;
            /**
             * @brief 精度半球。"E"表示东经，"W"表示西经。
             */
            std::string longitude_semi;
        };

    private:
        rtos::Semaphore _sem{1, 1};
        position_t _pos{};
        position_t _last_valid_pos{};

    private:
        /**
         * @brief 解析推荐的定位信息 $GPRMC。
         */
        void parse_gprmc(const std::vector<std::string>& frame)
        {
            _sem.acquire();
            _pos.is_valid =
                frame[2] == "A"; // "A" 表示有效定位，"V" 表示无效定位。
            _pos.latitude = frame[3];
            _pos.latitude_semi = frame[4];
            _pos.longitude = frame[5];
            _pos.longitude_semi = frame[6];

            if (_pos.is_valid)
            {
                _last_valid_pos = _pos;
            }
            _sem.release();
        }

    public:
        /**
         * @brief 获取当前的位置信息。
         *
         * @note 该函数是线程安全的。
         */
        position_t get_current_position()
        {
            position_t ret;
            _sem.acquire();
            ret = _pos;
            _sem.release();
            return ret;
        }
        /**
         * @brief 获取最后一次有效的位置信息。
         *
         * @note 该函数是线程安全的。
         */
        position_t get_last_valid_position()
        {
            position_t ret;
            _sem.acquire();
            ret = _last_valid_pos;
            _sem.release();
            return ret;
        }
    };
} // namespace peripheral
