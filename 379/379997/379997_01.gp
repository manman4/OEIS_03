M=20;

\\ a(n) = n! * Sum_{k=0..n} (k+1) * (k-2)^(n-k)/(n-k)!.
a(n) = n! * sum(k=0, n, (k+1) * (k-2)^(n-k) / (n-k)!);  
for(n=0, M, print1(a(n),", "));

