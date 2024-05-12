计划明年初跑路，这次开始提前准备算法题，坚持每日最少一题。
## 2024 0512
### 无重复字符的最长子串
这题就是最基本的滑动窗口
```C++
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        if (s.empty()) {
            return 0;
        }
        std::unordered_set<char> lookup;
        size_t left = 0;
        size_t max_length = 1;
        for (size_t i = 0; i < s.length(); ++i) {
            char c = s[i];
            // 移动左边界直到没有重复字符
            while (lookup.count(c) != 0) {
                lookup.erase(s[left]);
                left++;
            }
            max_length = std::max(max_length, i - left + 1);
            lookup.emplace(c);
        }
        return max_length;
    }
};
```

### 最长回文子串
这题主要是要想到中心扩算的计算方法，然后就是左右边界的处理
```C++
class Solution {
public:
    string longestPalindrome(string s) {
        if (s.empty()) {
            return "";
        }
        int32_t left = 0;
        int32_t right = 0;
        int32_t length = 1;
        for (int32_t i = 0; i < s.length(); ++i) {
            auto odd_len = Expand(s, i, i);
            auto even_len = Expand(s, i, i + 1);
            auto cur_length = std::max(odd_len, even_len);
            if (cur_length > length) {
                left = i - (cur_length - 1) / 2;    // 计算左边界
                right = i + cur_length / 2;         // 计算右边界
                length = cur_length;
            }
        } 
        return s.substr(left, length);
    }

    int32_t Expand(const std::string& str, int32_t left, int32_t right) {
        if (right < left) {
            return 0;
        }
        while (left >= 0 && right < str.length()) {
            if (str[left] != str[right]) {
                break;
            }
            left--;
            right++;
        }
        return right - left - 1;
    }
};
```

### Z 字形变换
这题没啥说的，主要是巧用一个flag，有点通过补码表示减法的意思
```C++
class Solution {
public:
    string convert(string s, int numRows) {
        int32_t row = 0;
        int32_t flag = -1;
        std::vector<std::string> records(numRows);
        for (const auto c : s) {
            records[row].append(1, c);
            if (row == 0 || row == numRows - 1) {
                flag = -flag;
            }
            row += flag;
        }
        std::string result;
        for (const auto& str : records) {
            result.append(str);
        }
        return result;
    }
};
```

### 整数反转
这题关于溢出的处理是比较重要的，也是好几道题常用的。这里有个细节，只是判断当前值*10是否溢出，为什么不需要判断余数呢？
因为小于2^31的10位数，首位只能是1或2，反转过来末位是1或2，小于7。如果大于7，输入就溢出了。所以不用考虑末位的7和-8，只要保证其余9位满足条件就行。
```C++
class Solution {
public:
    int reverse(int x) {
        int32_t value = x;
        int32_t result = 0;
        while (value != 0) {
            int32_t reminder = value % 10;
            if (INT32_MAX / 10 < result || INT32_MIN / 10 > result) {
                return 0;
            }
            result = result * 10 + reminder;
            value /= 10;
        }
        return result; 
    }
};
```

### 字符串转换整数 (atoi)
这题和上面比较类似，关键也是溢出的处理。
```C++
class Solution {
public:
    int myAtoi(string s) {
        if (s.empty()) {
            return 0;
        }
        int32_t index = 0;
        int32_t result = 0;
        while (s[index] == ' ') {
            index++;
        }
        int32_t flag = 1;
        if (s[index] == '-') {
            flag = -1;
            index++;
        } else if (s[index] == '+') {
            index++;
        }
        auto is_outof_boundary = [](int32_t cur, int32_t last, int32_t flag) {
            if (flag > 0) {
                if (INT32_MAX / 10 < cur) {
                    return true;
                }
                if (INT32_MAX / 10 == cur && last > INT32_MAX % 10) {
                    return true;
                }
            } else {
                if (abs(INT32_MIN / 10) < cur) {
                    return true;
                }
                // 其实这里的=不用加，正常运行没有问题，但是leetcode判题限制了运算不能溢出，INT32_MIN最后一位是8，而INT32_MAX
                // 最后一位是7，这里运算用的又都是正数，所以如果最后一位正好是8也要直接判断为溢出，输出结果是一样的
                if (abs(INT32_MIN / 10) == cur && last >= abs(INT32_MIN % 10)) {
                    return true;
                }
            }
            return false;
        };
        while (index < s.length()) {
            if (!isdigit(s[index])) {
                break;
            }
            int32_t val = s[index] - '0';
            if (is_outof_boundary(result, val, flag)) {
                return flag > 0 ? INT32_MAX : INT32_MIN;
            }
            result = result * 10 + val;
            index++;
        }
        return result * flag;
    }
};
```
### 回文数
这里最朴素的做法自然就是直接计算反转的结果，这里如果不允许使用更大的数据类型也要考虑溢出
class Solution {
public:
    int Reverse(int x) {
        int32_t value = x;
        int32_t result = 0;
        while (value != 0) {
            int32_t reminder = value % 10;
            if (INT32_MAX / 10 < result || INT32_MIN / 10 > result) {
                return 0;
            }
            result = result * 10 + reminder;
            value /= 10;
        }
        return result; 
    }
    bool isPalindrome(int x) {
        if (x < 0) {
            return false;
        }
        if (x < 10) {
            return true;
        }
        if (x % 10 == 0 ) {
            return false;
        }
        auto reverse = Reverse(x);
        return reverse == x;
    }
};
