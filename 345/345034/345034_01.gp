M=32;

a(n) = sum(k=1, n, (-2)^(n\k-1));
for(n=1, M, print1(a(n), ", "));