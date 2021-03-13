\\ \r
\\ Nは使わない 

a(n) = sum(k=1, n, eulerphi(k/gcd(n, k))); 
M=10000;
for(n=1, M, i=a(n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b332686.txt", n, " ", i))
