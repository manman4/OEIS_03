M=20;

\\ a(n) = n! * Sum_{k=1..n} (-1)^(k-1) * 2^(n-k)/k.
a(n) = n! * sum(k=1, n, (-1)^(k-1) * 2^(n-k)/k);
for(n=0, M, print1(a(n),", "))  