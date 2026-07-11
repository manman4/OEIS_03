\\ a(n) = Sum_{d|n} d^n * J_n(n/d), where J_k is the k-th Jordan totient function.
J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n) = sumdiv(n, d, d^n*J(n/d, n));
for(n=1, 20, print1(a(n), ", "));
