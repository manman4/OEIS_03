\\ \r
\\ Nは使わない 

v(n)={x='x+O('x^(n+10)); serlaplace(sqrt(exp(x)*(2-exp(x)))) };
M=500;
v=v(M);
for(n=0, M, i=polcoef(v, n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b348468_1.txt", n, " ", i))

