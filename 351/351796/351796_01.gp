M=17;

a(n) = n!*sum(k=0, n, (-k*(n-k))^k/k!);
for(n=0, M, print1(a(n), ", "));