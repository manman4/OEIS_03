M=100;

a(n) = sum(k=1, n, (-k)^k)%n;
for(n=1, M, print1(a(n), ", "));