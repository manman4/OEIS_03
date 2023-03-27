T(n, k) = sum(j=0, n, (-9)^j*binomial(-1/3, j)*binomial(k*j, n-j));
for(n=0, 9, for(k=0, n, print1(T(k,n-k),", ")))    