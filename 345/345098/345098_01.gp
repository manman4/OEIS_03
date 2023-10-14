M=18;

a(n) = sum(k=1, n, (n\k)^(n\k));
for(n=1, M, print1(a(n), ", "));