M=19;

\\ a(n) = Sum_{d|n} (-n)^(d-1).
a(n) = sumdiv(n, d, (-n)^(d-1));
for(n=1, M, print1(a(n), ", "));

\\ a(n) = [x^n] Sum_{k>=1} log(1 + n*x^k) / k.
b(n) = polcoef(sum(k=1, n, log(1 + n*x^k + x*O(x^n)) / k), n);
for(n=1, M, print1(a(n)-b(n), ", "));