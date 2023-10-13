M=19;

a(n) = sum(k=0, n, k!*fibonacci(k)*abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));