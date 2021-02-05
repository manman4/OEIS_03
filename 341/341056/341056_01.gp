a(n) = n!*(1+sum(j=1, n, sum(k=1, j, n^(j-k)*binomial(j-1, k-1)/k!)));
for(n=0, 20, print1(a(n), ", "))