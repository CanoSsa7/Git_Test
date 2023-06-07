#include <iostream>
using namespace std;
template <class Type>
class Traveling
{
    friend Type Tsp(int**, int[], int, Type);

public:
    void Backtrack(int i);
    // n→图G的顶点数，x→存放当前解的结点编号
    // bestx→存放最优解的结点编号
    int n, *x, *bestx;
    // a→图G的邻接矩阵，cc→当前已求得的费用
    // bestc-→当前已求的的最佳路径的最优值
    Type** a, cc, bestc;
};
template <class Type>
Type Tsp(Type** a, int v[], int n);

int main()
{
    int** a, * v, n, bestc;
    cin >> n;
    v = new int[n + 1];
    a = new int* [n + 1];
    for (int i = 0; i <= n; i++)
        a[i] = new int[n + 1];
    for (int i = 1; i <= n; i++)
        for (int j = i + 1; j <= n; j++)
        {
            cin >> a[i][j];
            a[j][i] = a[i][j];
        }
    bestc = Tsp(a, v, n);
    cout << bestc << endl;
    return 0;
}
template <class Type>
void Traveling<Type>::Backtrack(int i)
{
    int temp;
    if (i == n){
        if (a[x[n - 1]][x[n]] != 0 && a[x[n]][x[1]] != 0 &&
            (cc + a[x[n - 1]][x[n]] + a[x[n]][x[1]] < bestc || bestc == 0)) {
            // 循环将当前的最优路径保存到bestx中
            for (int j = 1; j <= n; j++)
                bestx[j] = x[j];
            bestc = cc + a[x[n - 1]][x[n]] + a[x[n]][x[1]];
        }
    }
        else {
            // 在剩余的顶点中寻找满足条件的结点，因此是从i号结点开始
            for (int j = i; j <= n; j++)
            {
                if (a[x[i - 1]][x[j]] != 0 && 
                (cc + a[x[i - 1]][x[j]] < bestc || bestc == 0))
                {
                    temp = x[i];
                    x[i] = x[j];
                    x[j] = temp;
                    cc += a[x[i - 1]][x[i]];
                    Backtrack(i + 1);
                    cc -= a[x[i - 1]][x[i]];
                    temp = x[i];
                    x[i] = x[j];
                    x[j] = temp;
                }
            }
        }
}
template <class Type>
Type Tsp(Type** a, int v[], int n)
{
    Traveling<Type> Y;
    // 初始化Y
    Y.x = new int[n + 1];
    // 存放x的初始值
    for (int i = 1; i <= n; i++)
    {
        Y.x[i] = i;
    }
    Y.a = a;
    Y.n = n;
    Y.bestc = 0;
    Y.bestx = v;
    Y.cc = 0;
    Y.Backtrack(2);
    delete[] Y.x;
    return Y.bestc;
}