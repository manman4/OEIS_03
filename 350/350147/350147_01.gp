M=51;

a(n) = sum(k=1, n, (n\(2*k-1))^k);
for(n=1, M, print1(a(n), ", "));