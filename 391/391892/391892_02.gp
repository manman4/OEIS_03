M=26;

\\ a(n) = Sum_{k=0..n} (k+1) * 2^(n-k) * binomial(2*k+1,2*n-2*k+1).
b(n) = sum(k=0, n, (k+1)*2^(n-k)*binomial(2*k+1, 2*n-2*k+1));
for(n=0, M, print1(b(n),", "));

\\ a(n) = 4*a(n-1) + 2*a(n-2) - 4*a(n-3) - 33*a(n-4) - 8*a(n-5) + 8*a(n-6) + 32*a(n-7) - 16*a(n-8).
a(n) = if(n<8, b(n), 4*a(n-1) + 2*a(n-2) - 4*a(n-3) - 33*a(n-4) - 8*a(n-5) + 8*a(n-6) + 32*a(n-7) - 16*a(n-8));
for(n=0, 20, print1(a(n)-b(n),", "));