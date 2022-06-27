M=21;

a(n) = sum(k=1, n, abs(stirling(n, k, 1))*binomial(2*k-2, k-1)/k);
for(n=1, M, print1(a(n), ", "));