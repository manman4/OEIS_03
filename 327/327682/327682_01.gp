\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); prod(k=1, n, (-1+sqrt(1+4*x^k))/(2*x^k))};
M=1000;
v=v(M);
for(n=0, M, i=polcoef(v, n); if((i<0)+#digits(i) > 1000, break); write("C:\\Users\\xxx\\Desktop\\b327682_1.txt", n, " ", i))
