T(n, k) = (-1)^n*sum(j=0, n\2, (-k)^(n-j)*binomial(n-j, j));
for(n=0, 11, for(k=0, n, print1(T(k, n-k), ", ")))