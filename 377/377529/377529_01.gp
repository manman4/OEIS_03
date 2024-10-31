M=19;

\\ a(n) = n! * Sum_{k=0..n} (k+1) * k^(n-k)/(n-k)!.
a(n) = n! * sum(k=0, n, (k+1) * k^(n-k)/(n-k)!);
for(n=0, M, print1(a(n), ", ")) 