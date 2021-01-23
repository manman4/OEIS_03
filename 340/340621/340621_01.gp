\\ \r
\\ Nは使わない 
default(parisize, 120000000);

v(n)={x='x+O('x^(n+10)); sum(k=1, sqrt(n), x^(k^2)*(1-x^(2*k))/prod(j=1, k, (1-x^(2*j))^2)) };
M=10000;
v=v(M);
for(n=0, M, write("/Users/xxx/Desktop/b340621.txt", n, " ", polcoef(v, n)))