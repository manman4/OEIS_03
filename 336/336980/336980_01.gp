\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); prod(k=1, n, 1+x^k*(1+k*x))};
M=1000;
v=v(M);
for(n=0, M, i=polcoef(v, n); if((i<0)+#digits(i) > 1000, break); write("C:\\Users\\xxx\\Desktop\\b336980.txt", n, " ", i))
