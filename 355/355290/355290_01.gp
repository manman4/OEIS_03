M=25;

a(n) = sum(k=0, n, (-1)^(n-k)*stirling(n, k, 2)*binomial(2*k, k)/(k+1));
for(n=0, M, print1(a(n), ", "));