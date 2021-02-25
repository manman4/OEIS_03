\\ \r
\\ Nは使わない 
v(n)={x='x+O('x^(n+10)); prod(k=1, n, 1+x^k*(1-x)^k) };
M=1000;
v=v(M);
for(n=0, M, write("C:\\Users\\xxx\\Desktop\\b307501.txt", n, " ", polcoef(v, n)))

