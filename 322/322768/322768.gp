\\ \r
\\ Nは使わない 

{T(n, k) = if(k==0, sum(j=0, n, stirling(n, j, 2)), (T(n+2, k-1)+T(n+1, k-1)+sum(j=0, k-1, binomial(k-1, j)*T(n, j)))/2)};
M=500;
for(n=0, M, write("C:\\Users\\xxx\\Desktop\\b322768.txt", n, " ", T(n, 3)))

