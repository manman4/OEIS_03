\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); sum(k=1, n, k*x^(k*(3*k-1)/2)/(1-x^k)) };
M=10000;
v=v(M);
for(n=1, M, write("C:\\Users\\xxx\\Desktop\\b330889.txt", n, " ", polcoef(v, n)))

