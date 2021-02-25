\\ \r
\\ Nは使わない 
v(n)={x='x+O('x^(n+10)); 1/prod(k=1, n, 1-x^k*(1-x)^k) };
M=1000;
v=v(M);
for(n=0, M, write("C:\\Users\\xxx\\Desktop\\b307500.txt", n, " ", polcoef(v, n)))

