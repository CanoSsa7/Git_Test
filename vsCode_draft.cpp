#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include <stack>
#include <functional>
using namespace std;

struct ListNode{
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};
struct TreeNode{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};
class Solution {
public:
    void quickSort(vector<int>& nums, int left, int right) {
            if(left >= right){
                return;
            }
            int pivotIndex = left + rand() % (right - left + 1);
            int pivot = nums[pivotIndex];
            swap(nums[pivotIndex], nums[left]);
            int i = left, j = right;
            while(i < j){
                while(nums[j] >= pivot && i < j){
                    j--;
                }
                while(nums[i] <= pivot && i < j){
                    i++;
                }
                swap(nums[i], nums[j]);                
            }
            swap(nums[i], nums[left]);
            quickSort(nums, left, i - 1);
            quickSort(nums, i + 1, right);
            return;
    }
    
};



int main() {
    Solution s1;

    vector<int> nums;
    
    for (int i = 0; i < 10;i++){
        nums.push_back(rand() % 10);
    }
    for(auto i:nums){
        cout << i << " ";
    }
    cout << endl;
    s1.quickSort(nums, 0, nums.size() - 1);
    cout << "Êä³ö" << endl;
    for(auto i:nums){
        
        cout << i << " ";
    }
    return 0;
}
