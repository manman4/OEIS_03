\\ T(n,k) = Sum_{d|n} d * J_k(d), where J_k is the k-th Jordan totient function.
J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
T(n, k) = sumdiv(n, d, d * J(d, k));
for(n=1, 10, for(k=1, n, print1(T(k, n-k+1), ", ")));

