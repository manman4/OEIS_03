\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); prod(k=1, n, (1+3*x^k+x^(2*k))/(1+2*x^k))};
M=1000;
v=v(M);
for(n=0, M, i=polcoef(v, n); if((i<0)+#digits(i) > 1000, break); write("C:\\Users\\xxx\\Desktop\\b327684_1.txt", n, " ", i))
