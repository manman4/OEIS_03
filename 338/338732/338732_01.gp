\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); sum(k=1, n, x^(k*(3*k+1))/(1-x^k)) };
M=10000;
v=v(M);
for(n=0, M, write("/Users/xxx/Desktop/b338732_gp_test.txt", n, " ", polcoef(v, n)))

