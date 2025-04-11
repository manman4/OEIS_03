M=30;

\\ Product_{j >= 1} (1 + x^j + y^j)
a284593(n, k) = my(x='x+O('x^(n+20)), y='y+O('y^(k+20))); polcoef(polcoef(prod(j=1, max(n, k)+10, 1 + x^j + y^j), n), k);
\\ a(n) = Sum_{k=0..n} 2^k * A284593(k,n-k).
a(n) = sum(k=0, n, 2^k * a284593(k, n-k));
for(n=0, M, print1(a(n), ", ")); 