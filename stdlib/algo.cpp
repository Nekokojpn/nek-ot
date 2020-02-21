#include <math.h>
#include <iostream>
#include <algorithm>
typedef long long ll;
const ll MOD = pow(10, 9) + 7;

ll gcd(ll a, ll b){
    if(a < b)
        std::swap(a,b);
    if(a%b == 0)
        return b;
    return gcd(a % b, b);
}
ll lcm(ll a,ll b){
    return a/gcd(a,b)*b;
}