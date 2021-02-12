\\ 1, 5, 16, 208, 688, 784, 2864, 9555, 17776, 81239, 

\\ A128981
\\ isok(n) = sum(k=1, n, Mod(k, n)^k)==0;

isok(n) = sum(k=1, n, Mod(k, n)^(n+1-k))==0;
for(n=1, 10^5, if(isok(n), print1(n, ", ")))