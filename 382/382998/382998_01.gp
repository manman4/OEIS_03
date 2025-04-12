M=19;

\\ a(n) = Sum_{d|n} mu(n/d) * (-n)^(d-1).
b(n) = sumdiv(n, d, moebius(n/d)*(-n)^(d-1));
for(n=1, M, print1(b(n), ", "));

\\ Main diagonal of A383011.
a(n, k) = -sumdiv(n, d, moebius(n/d)*(-k)^d)/n;
for(n=1, M, print1(b(n)-a(n, n), ", "));  

\\ a(n) = [x^n] Sum_{k>=1} mu(k) * log(1 + n*x^k) / k.
c(n) = polcoef(sum(k=1, n, moebius(k) * log(1 + n*x^k + x*O(x^n)) / k), n);
for(n=1, M, print1(b(n)-c(n), ", "));