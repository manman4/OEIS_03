\\ a(n) = Sum_{d|n} d * J_n(d), where J_k is the k-th Jordan totient function.
J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n) = sumdiv(n, d, d * J(d, n));
for(n=1, 10, print1(a(n), ", "));

