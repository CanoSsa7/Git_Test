#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <numeric>
#include <map>
#include <stack>
using namespace std;
struct ListNode{
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};
class Solution {
public:
    int maxArea(vector<int>& height) {
        
    }
};
int main() {
    Solution s1;
    cout<<s1.maxProduct({2,3,-2,4})<<endl;
}
