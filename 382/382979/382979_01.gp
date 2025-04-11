M=26;

\\ A(n,k) = [x^n * y^k] Product_{j>=1} 1/(1 - x^j + y^j).
a382974(n, k) = my(x='x+O('x^(n+20)), y='y+O('y^(k+20))); polcoef(polcoef(1/prod(j=1, max(n, k)+10, 1 - x^j + y^j), n), k);
for(n=0, M, print1(a382974(n, n), ", "));