M=32;

\\ a(n) = (1/4) * Sum_{k=0..floor(n/2)} (k+2) * 2^k * binomial(2*k+2,2*n-4*k+1).
b(n) = (1/4) * sum(k=0, n\2, (k+2) * 2^k * binomial(2*k+2, 2*n-4*k+1));
for(n=0, M, print1(b(n),", "));
\\ for(n=0, M, write("/Users/xxx/Desktop/b391833_1.txt", n, " ", a(n)));

\\ a(n) = 8*a(n-2) + 8*a(n-3) - 24*a(n-4) - 16*a(n-5) + 8*a(n-6) - 32*a(n-7) - 48*a(n-8) + 96*a(n-9) - 96*a(n-10) + 64*a(n-11) - 16*a(n-12).
a(n) = if(n<12, b(n), 8*a(n-2) + 8*a(n-3) - 24*a(n-4) - 16*a(n-5) + 8*a(n-6) - 32*a(n-7) - 48*a(n-8) + 96*a(n-9) - 96*a(n-10) + 64*a(n-11) - 16*a(n-12));
for(n=0, 25, print1(a(n)-b(n),", "));