\\ a(n) = Sum_{d|n} d * J_{n+1}(n/d), where J_k is the k-th Jordan totient function. 
J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n) = sumdiv(n, d, d*J(n/d, n+1));
for(n=1, 20, print1(a(n), ", "));
