v(n)={x='x+O('x^(n+10)); sum(k=1, sqrtint(n), x^k^2/(1-k*x^k)^(k+1)) };
M=5000;
v=v(M);
for(n=1, M, write("/Users/xxx/Desktop/b376017_1.txt", n, " ", polcoef(v, n)))