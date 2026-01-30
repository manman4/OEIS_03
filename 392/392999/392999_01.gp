M=18;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (n-k+1)^(n-2*k-1) * binomial(n,k)/(n-2*k)!.
a(n) = n! * sum(k=0, n\2, (n - k + 1)^(n - 2*k - 1) * binomial(n, k) / (n - 2*k)!);
for(n=0, M, print1(a(n), ", "));
