M=22;

a(n) = n!/6*sum(k=0, n-3, 1/k!);
for(n=0, M, print1(a(n), ", "));