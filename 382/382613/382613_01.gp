M=28;

\\ a(n) = Sum_{k=0..n} (k+1) * binomial(3*k,n-k).
b(n) = sum(k=0, n, (k+1)*binomial(3*k,n-k));
for(n=0, M, print1(b(n),", "));

\\ a(n) = 2*a(n-1) + 5*a(n-2) - 13*a(n-4) - 20*a(n-5) - 15*a(n-6) - 6*a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 2*a(n-1) + 5*a(n-2) - 13*a(n-4) - 20*a(n-5) - 15*a(n-6) - 6*a(n-7) - a(n-8));
for(n=0, 20, print1(a(n)-b(n),", "));


