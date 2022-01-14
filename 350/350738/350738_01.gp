default(parisize, 120000000)

\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); sum(k=0, sqrtint(n), (-1)^k*x^k^2*prod(j=1, k, 1+x^j))};
M=10000;
v=v(M);
for(n=0, M, write("/Users/xxx/Desktop/b350738_1.txt", n, " ", polcoef(v, n)))