\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); sum(k=1, n, x^(k*(2*k-1))/(1-x^k)) };
M=10000;
v=v(M);
for(n=1, M, write("C:\\Users\\xxx\\Desktop\\b334461.txt", n, " ", polcoef(v, n)))

