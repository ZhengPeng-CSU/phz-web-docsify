## 2024 0610
### 下一个排列
组合排列的系列题，反正我是想不到，只能看题解来理解。
```C++
class Solution {
public:
    void nextPermutation(vector<int>& nums) {
        int32_t i = nums.size() - 2;
        // 从右边开始找到不满足降序的位置
        while (i >= 0 && nums[i] >= nums[i + 1]) {
            i--;
        }
        // 如果没找到，说明此时数组是完全降序的，只能反转了
        if (i < 0) {
            reverse(nums.begin(), nums.end());
            return;
        }
        // 如果找到了此时的i就是右半部分 逆序的边界 - 1，也就是从i + 1开始都是降序的
        int32_t j = nums.size() - 1;
        // 从右边到i，在右边逆序区找到第一个比边界左边元素小的位置，比如123654，我们就会找到4，然后3和4交换
        // 为什么是这样呢？因为右边逆序了，不可能再组合更大的值出来，那只能修改左边的，因为是下一个排列，自然要
        // 修改尽可能的小，所以找到最小的和它叫唤。但是这时候得到的并不需写一个最近邻的，还有把右边重新升序，此时才是、
        // 修改了交换位后最小的了
        while (j > i && nums[j] <= nums[i]) {
            j--;
        }
        swap(nums[i], nums[j]);
        // 从i + 1开始全部顺序排列
        reverse(nums.begin() + i + 1, nums.end());
    }
};
```

### 组合总数
又是一个标准的dfs，所不同的是，递归的几个关键参数，路径，目标，当前位置，而这里递归的时候由于每一个元素可以无限选取，所以当前位置不用加1。每次递归为按对路径回溯，为了去重提前对数组排列。
```C++
class Solution {
public:
    std::vector<std::vector<int>> combinationSum(std::vector<int>& candidates, int target) {
        std::vector<std::vector<int>> result;
        if (candidates.empty()) {
            return result;
        }
        std::sort(candidates.begin(), candidates.end());
        std::function<void(int32_t, int32_t, std::vector<int32_t>&)> dfs = [&candidates, &result, target, &dfs] 
            (int32_t index, int32_t sum, std::vector<int32_t>& path) {
            if (sum == target) {
                result.emplace_back(path);
                return;
            }
            for (int32_t i = index; i < candidates.size(); ++i) {
                if (sum + candidates[i] > target) {
                    return;
                }
                path.emplace_back(candidates[i]);
                // 因为可以无限选取，index不能+1
                dfs(i, sum + candidates[i], path);
                path.pop_back();
            }
        };
        std::vector<int32_t> path;
        dfs(0, 0, path);
        return result;
    }
};
```

这题还有一种要求就是每一个元素只能选一次，自然的我们就能想到去重，去重还是一样的和三数之和一样的做法，i > start && nums[i] == nums[i - 1]。这里一定要注意，去重我们都是在排序后用i和i-1比较，不能用i和i+1比较，因为这样就错过了前面的元素。
```C++
class Solution {
public:
    vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
        std::vector<std::vector<int>> result;
        if (candidates.empty()) {
            return result;
        }
        std::sort(candidates.begin(), candidates.end());
        std::function<void(int32_t, int32_t, std::vector<int32_t>&)> dfs = [&candidates, &result, target, &dfs] 
            (int32_t index, int32_t sum, std::vector<int32_t>& path) {
            if (sum == target) {
                result.emplace_back(path);
                return;
            }
            for (int32_t i = index; i < candidates.size(); ++i) {
                if (sum + candidates[i] > target) {
                    return;
                }
                // 为什么不能用i + 1与i判断呢，因为i直接跳过会错过当前元素
                if(i > index && candidates[i] == candidates[i-1]){
                    continue;
                }
                path.emplace_back(candidates[i]);
                dfs(i + 1, sum + candidates[i], path);
                path.pop_back();
            }
        };
        std::vector<int32_t> path;
        dfs(0, 0, path);
        return result;
    }
};  
```
