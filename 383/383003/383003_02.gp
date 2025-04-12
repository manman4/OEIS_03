M=19;

\\ a(n) = [x^n] Sum_{k>=1} mu(k) * x^k / (1 + n*x^k).
a(n) = polcoef(sum(k=1, n, moebius(k) * x^k / (1 + n*x^k + x*O(x^n))), n);
for(n=1, M, print1(a(n), ", "));