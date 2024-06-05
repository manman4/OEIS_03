tabl(n) = my(nn=vector(n)); forperm([1..n], p, if(sum(k=1, n, (-1)^k*binomial(n-1, k-1)*p[k])==0, nn[p[1]]++)); nn;
for(n=1, 10, print(tabl(n)));