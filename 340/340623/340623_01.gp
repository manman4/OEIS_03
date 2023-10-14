\\ \r
\\ Nは使わない 
default(parisize, 120000000);

v(n)={x='x+O('x^(n+10)); prod(k=1, n, (1+x^(2*k-1))/(1-x^(2*k)))-sum(k=0, sqrt(n), x^(k^2)/prod(j=1, k, (1-x^(2*j))^2)) };
M=10000;
v=v(M);
for(n=0, M, write("/Users/xxx/Desktop/b340623_gp_test.txt", n, " ", polcoef(v, n)))