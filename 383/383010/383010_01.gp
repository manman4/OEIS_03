M=19;

\\ a(n) = -Sum_{d|n} (-n)^d.
a(n) = -sumdiv(n, d, (-n)^d);
for(n=1, M, print1(a(n), ", "));

