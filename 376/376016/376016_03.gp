v(n)={x='x+O('x^(n+10)); sum(k=1, sqrtint(n), (k*x^k)^k / (1 - k*x^k)^(k+1)) };
M=5000;
v=v(M);
for(n=1, M, write("/Users/xxx/Desktop/b376016_1.txt", n, " ", polcoef(v, n)))