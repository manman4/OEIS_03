\\ a(n) = Sum_{d|n} d^5 * J_2(n/d) 
J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n) = sumdiv(n, d, d^5*J(n/d, 2));
for(n=1, 20, print1(a(n), ", "));

\\ Sum_{d|n} d^2 * J_5(n/d), where J_k is the k-th Jordan totient function.
b(n) = sumdiv(n, d, d^2*J(n/d, 5));
for(n=1, 20, print1(a(n)-b(n), ", "));
