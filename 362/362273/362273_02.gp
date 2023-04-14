M=22;

a(n) = n! * sum(i=0, n, (-1)^(n-i) * sum(j=0, n-i, i^j * j^(n-i-j) / (j! * (n-i-j)!)));
for(n=0, M, print1(a(n), ", ")); 