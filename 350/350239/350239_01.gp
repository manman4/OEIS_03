M=103;

a(n) = sum(k=1, n\2, (-1)^(k+1)*(n^k\(2*k)^k));
for(n=1, M, print1(a(n), ", "));