M=17;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} (3*k)^(n-k) * (n+k)!/(k! * (n-k)!).
a(n) = (1/(n+1)) * sum(k=0, n, (3*k)^(n-k) * (n+k)!/(k! * (n-k)!));
for(n=0, M, print1(a(n),", "));

