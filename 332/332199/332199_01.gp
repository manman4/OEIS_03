\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); 1/prod(i=1, n, prod(j=1, n\i, 1-i*x^(i*j)))};
M=1000;
v=v(M);
for(n=0, M, i=polcoef(v, n); if((i<0)+#digits(i) > 1000, break); write("C:\\Users\\xxx\\Desktop\\b332199_1.txt", n, " ", i))
