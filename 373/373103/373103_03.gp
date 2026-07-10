\\ a(n) = Sum_{d|n} d^4 * J_5(d), where J_k is the k-th Jordan totient function.
J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n) = sumdiv(n, d, d^4 * J(d, 5));
for(n=1, 30, print1(a(n), ", "));

