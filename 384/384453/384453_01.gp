M=11;

\\ a(n) = Product_{k=1..n} (1 - (-n)^k)/(1 + n).
a(n) = prod(k=1, n, (1-(-n)^k)/(1+n));
for(n=0, M, print1(a(n),", "));



