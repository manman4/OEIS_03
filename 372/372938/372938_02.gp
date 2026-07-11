\\ T(n,k) = Sum_{d|n} d^k * J_k(n/d), where J_k is the k-th Jordan totient function.
J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
T(n, k) = sumdiv(n, d, d^k*J(n/d, k));
for(n=1, 10, for(k=1, n, print1(T(k, n-k+1), ", ")));
