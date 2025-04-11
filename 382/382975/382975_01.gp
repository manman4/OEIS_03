M=11;

\\ A(n,k) = [x^n * y^k] Product_{j>=1} (1 + x^j - y^j).
a382975(n, k) = my(x='x+O('x^(n+20)), y='y+O('y^(k+20))); polcoef(polcoef(prod(j=1, max(n, k)+10, 1 + x^j - y^j), n), k);
for(n=0, M, for(k=0, n, print1(a382975(k, n-k), ", ")));