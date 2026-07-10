M=40;

\\ a(n) = Sum_{d|n} J_2(n/d) * (n/d) * sigma_2(d^2)/sigma(d^2), where J_k is the k-th Jordan totient function.
J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n) = sumdiv(n, d, J(n/d, 2)*n/d*sigma(d^2, 2)/sigma(d^2));
for(n=1, M, print1(a(n), ", "));

