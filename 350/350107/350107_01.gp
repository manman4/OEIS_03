M=48;

a(n) = sum(k=1, n, k*(n\k)^2);
for(n=1, M, print1(a(n), ", "));