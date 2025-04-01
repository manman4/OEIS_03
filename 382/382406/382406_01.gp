M=29;

\\ a(n) = Sum_{k=0..n} binomial(k+2,2) * binomial(2*k,n-k).
b(n) = sum(k=0, n, binomial(k+2,2)*binomial(2*k,n-k));
for(n=0, M, print1(b(n),", "));

\\ a(n) = 3*a(n-1) + 3*a(n-2) - 8*a(n-3) - 12*a(n-4) + 3*a(n-5) + 17*a(n-6) + 15*a(n-7) + 6*a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 3*a(n-1) + 3*a(n-2) - 8*a(n-3) - 12*a(n-4) + 3*a(n-5) + 17*a(n-6) + 15*a(n-7) + 6*a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n),", "));
