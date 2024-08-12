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
```C++
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
```

## 2024 0513
### 盛最多水的容器
这题就是最基本的双指针，但是没做过感觉就很难想到，主要是移动左右指针的时机。
```C++
class Solution {
public:
    int maxArea(vector<int>& height) {
        if (height.size() < 2) {
            return 0;
        }
        int32_t left = 0;
        int32_t right = height.size() - 1;
        int32_t result = 0;
        while (left < right) {
            auto cur = (right - left) * std::min(height[left], height[right]);
            result = std::max(result, cur);
            if (height[left] < height[right]) {
                left++;
            } else {
                right--;
            }
        }
        return result;
    }
};
```

### 整数转罗马数字 罗马数字转整数
整数转罗马数字，主要是需要明白这个规则，先建立一个数字对应的符号表，每次取尽可能大的值，循环减下去。而罗马数字转整数比较简单，从左到右依次相加，注意判断如果右边的比左边的大说明是特殊的。
```C++
class Solution {
public:
    string intToRoman(int num) {
        static const pair<int, string> value_symbols[] = {
            {1000, "M"},
            {900,  "CM"},
            {500,  "D"},
            {400,  "CD"},
            {100,  "C"},
            {90,   "XC"},
            {50,   "L"},
            {40,   "XL"},
            {10,   "X"},
            {9,    "IX"},
            {5,    "V"},
            {4,    "IV"},
            {1,    "I"},
        };
        std::string result;
        for (const auto& [value, symbol] : value_symbols) {
            while (num >= value) {
                result.append(symbol);
                num -= value;
            }
            if (num == 0) {
                break;
            }
        }
        return result;
    }

    int romanToInt(const string& s) {
        std::unordered_map<char, int32_t> symbol_value = {
            {'M', 1000},
            {'D', 500},
            {'C', 100},
            {'L', 50},
            {'X', 10},
            {'V', 5},
            {'I', 1},
        };
        if (s.empty()) {
            return 0;
        }
        auto last = 0;
        int32_t result = 0;
        for (const auto c : s) {
            if (symbol_value[c] > last) {
                result = result - last * 2 + symbol_value[c];
            } else {
                result += symbol_value[c];
            }
            last = symbol_value[c];
        }
        return result;
    }
};
```

## 2024 0514
### 三数之和
先记录下我的直觉做法，三数固定一个数，然后转两数之和，关键就在于去重，先排序，然后如果当前与上一个值相同，最外圈就跳过，内圈要判断下上一个值是否满足要求了，如果满足就跳过，不满足就不跳过。
```C++
class Solution {
public:
    std::vector<std::vector<int>> threeSum(std::vector<int>& nums) {
        return ThreeSum(nums, 0);
    }

    std::vector<std::vector<int32_t>> ThreeSum(std::vector<int32_t> nums, int32_t target) {
        sort(nums.begin(), nums.end());
        std::unordered_map<int32_t, int32_t> lookup;
        std::vector<std::vector<int32_t>> result;
        for (int32_t i = 0; i < nums.size() - 2; ++i) {
            if (i > 0 && nums[i] == nums[i - 1]) {
                continue;
            }
            int32_t cur_target = target - nums[i];
            lookup.clear();
            bool last_record = false;
            for (int32_t j = i + 1; j < nums.size(); ++j) {
                if (j > i + 1 && nums[j] == nums[j - 1] && last_record) {
                    continue;
                }
                last_record = false;
                auto diff = cur_target - nums[j];
                if (auto iter = lookup.find(diff); iter != lookup.end()) {
                    result.push_back({nums[i], nums[iter->second], nums[j]});
                    last_record = true;
                }
                lookup[nums[j]] = j;
            }
        }
        return result;
    }
};
```

当然，这题最经典的做法还是双指针和去重，这样想来，两数之后也可以直接排序后双指针。
```C++
class Solution {
public:
    std::vector<std::vector<int>> threeSum(std::vector<int>& nums) {
        return ThreeSum(nums, 0);
    }

    std::vector<std::vector<int32_t>> ThreeSum(std::vector<int32_t> nums, int32_t target) {
        sort(nums.begin(), nums.end());
        std::vector<std::vector<int32_t>> result;
        for (int32_t i = 0; i < nums.size() - 2; ++i) {
            if (i > 0 && nums[i] == nums[i - 1]) {
                continue;
            }
            // 因为排序了，最后两个数是最大的，如果加起来都没target大，肯定中间的都不满足要求的
            if (nums[i] + nums[i + 1] + nums[i + 2] > target) {
                break;
            }
            int32_t left = i + 1;
            int32_t right = nums.size() - 1;
            while (left < right) {
                auto sum = nums[i] + nums[left] + nums[right];
                if (sum == target) {
                    result.push_back({nums[i], nums[left], nums[right]});
                    // 这里的关键是去重的处理，只有当前值满足要求了才能去重，和我那个last record其实异曲同工
                    while (left < right && nums[left] == nums[left + 1]) {
                        left++;
                    }
                    while (right > left && nums[right] == nums[right - 1]) {
                        right--;
                    }
                    left++;
                    right--;
                } else if (sum > target) {
                    right--;
                } else {
                    left++;
                }
            }
        }
        return result;
    }
};
```

## 2024 0516
### 最接近的三数之和
和三数之和的做法是一样的，但是这里的提前优化需要特别注意，不能直接return和continue了。
```C++
class Solution {
public:
    int threeSumClosest(vector<int>& nums, int target) {
        if (nums.empty()) {
            return 0;
        }
        sort(nums.begin(), nums.end());
        int32_t result = 0;
        int32_t delta = INT_MAX;
        for (int32_t i = 0; i < nums.size() - 2; ++i) {
            if (i > 0 && nums[i] == nums[i - 1]) {
                continue;
            }
            auto cur_min = nums[i] + nums[i + 1] +nums[i + 2];
            if (cur_min > target) {
                if (cur_min - target < delta) {
                    return cur_min;
                }
            }
            auto cur_max = nums[i] + nums[nums.size() - 2] + nums[nums.size() - 1];
            if (cur_max < target) {
                if (target - cur_max < delta) {
                    delta = target - cur_max;
                    result = cur_max;
                }
                continue;
            }

            int32_t left = i + 1;
            int32_t right = nums.size() - 1;
            while (left < right) {
                auto sum = nums[left] + nums[right] + nums[i];
                int32_t new_delta = abs(target - sum);
                if (new_delta == 0) {
                    return target;
                } 
                if (new_delta < delta) {
                    result = sum;
                    delta = new_delta;
                }
                if (sum < target) {
                    left++;
                } else {
                    right--;
                }
            }
        }
        return result;
    }
};
```


## 2024 0517
### 电话号码的字母组合
这题可以说是最标准的回溯了，也可以说是深度优先遍历的做法，这里的回溯，起始就是在当前调用递归函数前 + 字符，调用后再 - 字符，不过这一步可以直接放到形参里面。
```C++
class Solution {
public:
    std::unordered_map<char, std::string>& GetLookup() {
        static std::unordered_map<char, std::string> lookup = {
            {'2', "abc"}, 
            {'3', "def"}, 
            {'4', "ghi"}, 
            {'5', "jkl"}, 
            {'6', "mno"}, 
            {'7', "pqrs"}, 
            {'8', "tuv"}, 
            {'9', "wxyz"}
        };
        return lookup;
    }
    std::vector<std::string> letterCombinations(const std::string& digits) {
        std::vector<std::string> result;
        if (digits.empty()) {
            return result;
        }
        DFS(digits, 0, "", result);
        return result;
    }

    void DFS(const std::string& digits, int32_t index, const std::string& cur_str, std::vector<std::string>& result) {
        if (index == digits.size()) {
            result.emplace_back(cur_str);
            return;
        }
        for (const auto c : GetLookup()[digits[index]]) {
            DFS(digits, index + 1, cur_str + c, result);
        }
    }
};
```

在写这题的时候，还从评论区学到了一个lambda递归的方法，惊到我了，才知道还有这种写法。
```C++
class Solution {
public:
    std::vector<std::string> letterCombinations(const std::string& digits) {
        std::vector<std::string> result;
        if (digits.empty()) {
            return result;
        }
        std::unordered_map<char, std::string> lookup = {
            {'2', "abc"}, 
            {'3', "def"}, 
            {'4', "ghi"}, 
            {'5', "jkl"}, 
            {'6', "mno"}, 
            {'7', "pqrs"}, 
            {'8', "tuv"}, 
            {'9', "wxyz"}
        };
        std::function<void(int32_t, const std::string&)> dfs = [&lookup, &digits, &result, &dfs](int32_t index, const std::string& cur_str) {
            if (index == digits.size()) {
                result.emplace_back(cur_str);
                return;
            }
            for (const auto c : lookup[digits[index]]) {
                dfs(index + 1, cur_str + c);
            }
        };

        dfs(0, "");
        return result;
    }
};
```

## 2024 0518
### 四数之和
这题其实和三数之和完全一样，要是面试的时候能让我复制一下本地代码就好了，毕竟实际开发大部分可以重用的或者功能类似的代码也都是复制过来改的-_-
```C++
class Solution {
public:
    std::vector<std::vector<int>> fourSum(std::vector<int>& nums, int target) {
        std::vector<std::vector<int>> result;
        if (nums.size() < 4) {
            return result;
        }
        sort(nums.begin(), nums.end());
        for (int32_t first = 0; first < nums.size() - 3; ++first) {
            if (first > 0 && nums[first] == nums[first - 1]) {
                continue;
            }
            if (static_cast<int64_t>(nums[first]) + nums[first + 1] + nums[first + 2] + nums[first + 3] > target) {
                break;
            }
            if (static_cast<int64_t>(nums[first]) + nums[nums.size() - 1] + nums[nums.size() - 2] + nums[nums.size() - 3] < target) {
                continue;
            }
            for (int32_t second = first + 1; second < nums.size() - 2; ++second) {
                if (second > first + 1 && nums[second] == nums[second - 1]) {
                    continue;
                }
                if (static_cast<int64_t>(nums[first]) + nums[second] + nums[second + 1] + nums[second + 2] > target) {
                    break;
                }
                if (static_cast<int64_t>(nums[first]) + nums[second] + nums[nums.size() - 1] + nums[nums.size() - 2] < target) {
                    continue;
                }
                int32_t left = second + 1;
                int32_t right = nums.size() - 1;
                while (left < right) {
                    int64_t sum = static_cast<int64_t>(nums[left]) + nums[right] + nums[second] + nums[first];
                    if (sum == target) {
                        result.push_back({nums[first], nums[second], nums[left], nums[right]});
                        while (left < right && nums[left] == nums[left + 1]) {
                            left++;
                        }
                        while (right > left && nums[right] == nums[right - 1]) {
                            right--;
                        }
                        left++;
                        right--;
                    }else if (sum > target) {
                        right--;
                    } else {
                        left++;
                    }
                }
            }
        }
        return result;
    }
};
```

### 删除链表的倒数第 N 个结点
这题是快慢指针的典型引用，也是让我了解快慢指针的启蒙题了->，涉及到链表的题目，往往需要针对要操作的是否是头结点进行判断，
```C++
class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        if (head == nullptr) {
            return nullptr;
        }
        if (head->next == nullptr && n == 1) {
            return nullptr;
        }
        auto fast = head;
        auto slow = head;
        int32_t count = 0;
        while (count < n && fast != nullptr) {
            fast = fast->next;
            count++;
        }
        if (fast == nullptr) {
            return head->next;
        }
        while (fast->next != nullptr) {
            fast = fast->next;
            slow = slow->next;
        }
        if (slow && slow->next) {
            slow->next = slow->next->next;
        }
        return head;
    }
};
```
然而，如果用上哑结点的技巧，那就简单多了，全部统一对待，不需要特例判断，
```C++
class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        if (head == nullptr) {
            return nullptr;
        }
        // 添加一个哑节点
        auto dummy_head = std::make_unique<ListNode>();
        dummy_head->next = head;
        auto fast = dummy_head.get();
        auto slow = dummy_head.get();
        int32_t count = 0;
        while (count < n && fast != nullptr) {
            fast = fast->next;
            count++;
        }
        while (fast->next != nullptr) {
            fast = fast->next;
            slow = slow->next;
        }
        if (slow && slow->next) {
            slow->next = slow->next->next;
        }
        // 注意返回哑节点的下一个
        return dummy_head->next;
    }
};
```


## 2024 0519
### 合并两个有序链表
这题就没啥特别技巧，就是正常的链表操作，由于两个链表不等长，我们可以在一个循环里面判断其中一个结束了就结束循环，连接上剩下的，也可以在循环里面判空处理。看来以前做题的提交记录，说实话代码风格变化了很多，以前做题追求什么简洁行数上，现在是要把逻辑描述清楚。
```C++
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        auto dumpy_head = new ListNode();
        auto cur = dumpy_head;
        auto p1 = list1;
        auto p2 = list2;
        while (p1 != nullptr || p2 != nullptr) {
            auto val1 = INT32_MAX;
            auto val2 = INT32_MAX;
            auto cur_val = 0;
            if (p1 != nullptr) {
                val1 = p1->val;
            }
            if (p2 != nullptr) {
                val2 = p2->val;
            }
            if (val1 < val2) {
                cur_val = val1;
                p1 = p1 != nullptr ? p1->next : nullptr;
            } else {
                cur_val = val2;
                p2 = p2 != nullptr ? p2->next : nullptr;
            }
            cur->next = new ListNode(cur_val);
            cur = cur->next;
        }
        return dumpy_head->next;
    }
};
```

## 2024 0519
### 括号生成
这题其实也是深度遍历，首先我能想到的最简单的做法就是全部遍历，每次结束以后判断当前的字符串是否满足要求，怎么判断呢？当然是有效的括号了！
```C++
class Solution {
public:
    bool IsValid(const std::string& s) {
        if (s.empty()) {
            return false;
        }
        static std::unordered_map<char, char> match = {
            {')', '('},
            {'}', '{'},
            {']', '['},
        };
        std::vector<char> record;
        for (const auto c : s) {
            if (record.size() > 0 && record.back() == match[c]) {
                record.pop_back();
            } else {
                record.emplace_back(c);
            }
        }
        return record.empty();
    }
    std::vector<std::string> generateParenthesis(int32_t n) {
        std::vector<std::string> result;
        int32_t total_len = n * 2;
        std::string choice = "()";
        std::function<void(const std::string&, int32_t)> dfs = [this, &dfs, &result, &choice, total_len](const std::string& path, int32_t index) {
            if (index == total_len) {
                if (IsValid(path)) {
                    result.emplace_back(path);
                }
                return;
            }
            for (const auto c : choice) {
                dfs(path + c, index + 1);
            }
        };
        dfs("", 0);
        return result;
    }
};
```

再高端一点，在dfs的时候就要判断一下当前的括号能不能加，
```C++
std::vector<std::string> generateParenthesis(int32_t n) {
    std::vector<std::string> result;
    std::function<void(const std::string&, int32_t, int32_t)> dfs = [this, &dfs, &result, n](const std::string& path, int32_t left, int32_t right) {  
        if (left > n || right > n) {
            return;
        } 
        if (left == n && right == n) {
            result.emplace_back(path);
            return;
        }
        // 左括号小于n个，还可以添加左括号
        if (left < n) {
            dfs(path + '(', left + 1, right);
        }
        // 右括号要小于n个，且小于左括号才可添加
        if (right < n && right < left) {
            dfs(path + ')', left, right + 1);
        }
    };
    dfs("", 0, 0);
    return result;
}
```

## 2024 0520
### 两两交换链表中的节点
这题就很考验链表操作的基本功了，最主要的技巧还是哑节点，关键是要理解两两交换，也就是dummy->p1->p2->nextp1->nextp2变成dummy->p2->p1->nextp1->nextp2，然后dummy改成nextp1，继续迭代。
```C++
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        if (head == nullptr) {
            return nullptr;
        }
        // dummy->p1->p2->nextp1->nextp2
        // dummy->p2->p1->nextp1->nextp2
        auto dummy_head = std::make_unique<ListNode>();
        dummy_head->next = head;
        auto dummy = dummy_head.get();
        while (dummy->next && dummy->next->next) {
            auto p1 = dummy->next;
            auto p2 = dummy->next->next;
            dummy->next = p2;
            p1->next = p2->next;
            p2->next = p1;
            dummy = p1;
        }
        return dummy_head->next;
    }
};
```
这题的递归做法说实话我自己肯定是想不到的，递归太抽象了，
```C++
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        if (!head || !head->next) {
            return head;
        }
        // p1   ->   p2     ->    p3
        // head      new_head
        auto p2 = head->next;
        head->next = swapPairs(p2->next); //p1->next = p2-next
        p2->next = head;                  //p2->next = p1
        return p2;
    }
};
```

## 2024 0526
尴尬，一连好几天都没刷了，也就刷了几个简单的题，没必要记录，今天继续坚持。
###  两数相除
这题没啥可想的，数学操作咱们看过理解就行了，没必要自己去推导，这题的解法是倍增，就是累加的sum每次翻倍。
```C++
class Solution {
public:
    int divide(int dividend, int divisor) {
        if (dividend == 0) {
            return 0;
        }
        if (dividend == INT32_MIN) {
            if (divisor == 1) {
                return INT32_MIN;
            }
            if (divisor == -1) {
                return INT32_MAX;
            }
        }
        auto flag = 1;
        // 负数的范围更大，都转成负数处理
        if (dividend > 0) {
            dividend = -dividend;
            flag = -flag;
        }
        if (divisor > 0) {
            divisor = -divisor;
            flag = -flag;
        }
        int32_t result = 0;
        // 这里要有等号，因为相等的时候结果是1
        while (dividend <= divisor) {
            int32_t count = 1;
            int32_t sum = divisor;
            // 提前判断是否需要再加一次，否则退出去还要count退回一步
            while (dividend - sum < sum) {
                // sum每次翻倍
                sum += sum;
                count += count;
            }
            dividend -= sum;
            result += count;
        }
        return result * flag;
    }
};
```
与这题有异曲同工之妙的是快速幂，Pow(x, n)
```C++
class Solution {
public:
    double myPow(double x, int n) {
        if (x == 0) {
            return 0;
        }
        if (n == 0) {
            return 1;
        }
        // 如果n是int32_min，直接取反会导致溢出
        auto positive_pow = [](double x, int64_t exponent) {
            double result = 1.0;
            double base = x;
            while (exponent > 0) {
                // 奇数判断
                if (exponent & 1) {
                    result = result * base;
                }
                base = base * base;
                exponent = exponent / 2;
            }
            return result;
        };
        if (n < 0) {
            int64_t exponent = n;
            return 1 / positive_pow(x, -exponent);
        }
        return positive_pow(x, n);
    }
};
```
还有一题平方根，不过这题是使用二分查找的思想，其实现目标上看都是与二分类似，把n的复杂度变成对数复杂度。
```C++
class Solution {
public:
    int mySqrt(int x) {
        if (x == 0 || x == 1) {
            return x;
        }
        int32_t left{0};
        // 一般二分是右边界，这里因为平方根肯定不大于x/2，所以右边界不是x
        int32_t right = x / 2;
        int32_t result = 0;
        while (left <= right) {
            int32_t mid = left + (right - left) / 2;
            int64_t tmp = static_cast<int64_t>(mid) * mid;
            // 这与二分找数不同，这里是判断数的平方
            if (tmp == x) {
                return mid;
            }
            if (tmp > x) {
                right = mid - 1;
            } else {
                left = mid + 1;
                result = mid;
            }
        }
        return result;
    }
};
```

## 2024 0610
前面两周没怎么刷，上班太累了，各种乱七八糟加职场冷暴力，唉，打个工真难，如果可以的话，真想一直躺平摆烂。算了，继续刷吧。接下来是排序数组相关的题，第一次刷的时候感觉比较难理解，主要还是对二分法的应用，先来看个简单的。
### 寻找旋转排序数组中的最小值
这题最主要的是通过二分找到左右两部分的分割点，分割点肯定是最小的元素，每次二分的中点与当前的right进行比较。以第一次为例，如果小于right，证明此时的分割点肯定在中点的左边，因为左半部分都比right大，自然也比中点大。相反，则分割点位于中点的右边，我们借此移动左右边界。问题就是在于边界的判断上，此时我们不能使用等号，只能判断left < right，且移动右边界时只能是mid而不能是mid - 1，否则可能错过分割点，因为我们取mid时是靠近left的。
```C++
class Solution {
public:
    int findMin(vector<int>& nums) {
        if (nums.empty()) {
            return 0;
        }
        int32_t left = 0;
        int32_t right = nums.size() - 1;
        while (left < right) {
            int32_t mid = left + (right - left) / 2;
            if (nums[mid] > nums[right]) {
                left = mid + 1; 
            } else {
                right = mid;
            }
        }
        return nums[left];
    }
};
```
### 搜索旋转排序数组
这题最简单直接的思路就是二分两次，第一次二分找到分割点，第二次找到目标。第二次二分要注意做一下判断，
```C++
class Solution {
public:
    int search(vector<int>& nums, int target) {
        if (nums.empty()) {
            return -1;
        }
        if (target == nums[0]) {
            return 0;
        }
        int32_t left = 0;
        int32_t right = nums.size() - 1;
        while (left < right) {
            int32_t mid = left + (right - left) / 2;
            if (nums[mid] > nums[right]) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        // 第一次二分结束分割点就是left
        int32_t min_index = left;
        if (target == nums[min_index]) {
            return min_index;
        }
        if (min_index == 0) {
            // 第一个元素就是最小值，此时说明数组没有旋转，我们只需要直接二分
            left = 0;
            right = nums.size() - 1;
        } else {
            // target比第一个元素大，它只能位于左半部分
            if (target > nums[0]) {
                left = 0;
                right = min_index - 1;
            } else {
                // target比第一个元素小，它只能位于右半部分
                right = nums.size() - 1;
                left = min_index;
            }
        }
        // 第二次就是正常二分了
        while (left <= right) {
            int32_t mid = left + (right - left) / 2;
            if (nums[mid] == target) {
                return mid;
            } else if (nums[mid] > target) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return -1;
    }
};
```

既然谈到了二分，自然就要总结下我喜欢的标准的二分写法，判断条件是left <= right，然后mid = left + (right - left) / 2，比如最简单的有序数组查找，
```C++
class Solution {
public:
    int searchInsert(vector<int>& nums, int target) {
        int32_t left{0};
        int32_t right = nums.size() - 1;
        while (left <= right) {
            auto mid = left + (right - left) / 2;
            if (nums[mid] == target) {
                return mid;
            } else if (nums[mid] > target) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return left;
    }
};
```
当然，还有我们经常使用的边界查找，也就是第一个大于target的和最后一个大于target的，
```C++
class Solution {
public:
    vector<int> searchRange(vector<int>& nums, int target) {
        if (nums.empty()) {
            return {-1, -1};
        }
        if (nums[nums.size() - 1] < target || nums[0] > target) {
            return {-1, -1};
        }
        int32_t left{0};
        int32_t right = nums.size() - 1;
        // 第一次二分找第一个大于等于target的，也就是左边界，收缩右边，所以是mid >= target
        while (left <= right) {
            auto mid = left + (right - left) / 2;
            if (nums[mid] >= target) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        if (left > nums.size() - 1 || nums[left] != target) {
            return {-1, -1};
        }
        std::vector<int32_t> result{left, left};
        left = 0;
        right = nums.size() - 1;
        // 第二次二分找最后一个大于target的，也就是右边界，收缩左边，mid <= target
        while (left <= right) {
            auto mid = left + (right - left) / 2;
            if (nums[mid] <= target) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        result[1] = right;
        return result;
    }
};
```
我们平时使用的stl的lower_bound与这里的第一次二分是类似的，找到大于等于target的最小位置，而upper_bound是找到大于target的最小位置。

### 外观数列
这题刚开始题目都没看懂，不过思路不难，就是循环，唯一需要注意的是每次内层结束要补上最后的一组元素统计结果，
```C++
class Solution {
public:
    string countAndSay(int n) {
        std::string result = "1";
        for (int32_t i = 1; i < n; ++i) {
            auto c = result[0];
            int32_t count = 1;
            std::string cur_result;
            for (int32_t j = 1; j < result.size();j++) {
                if (result[j] == c) {
                    count++;
                } else {
                    cur_result.append(std::to_string(count)).append(1, c);
                    c = result[j];
                    count = 1;
                }
            }
            // 最后一组元素没有统计，需要单独加上
            cur_result.append(std::to_string(count)).append(1, c);
            result = cur_result;
        }
        return result;
    }
};
```