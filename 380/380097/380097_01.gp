M=16;

\\ a(n) = (1/(n+1)) * Sum_{k=0..n} 3^k * k^(n-k) * (n+k)!/(k! * (n-k)!).
a(n) = sum(k=0, n, 3^k * k^(n-k) * (n+k)!/(k! * (n-k)!))/(n+1);
for(n=0, M, print1(a(n),", "));

\\ a(n) == 0 (mod 3) for n>0.
for(n=0, 100, print1(a(n) % 3, ", "));

