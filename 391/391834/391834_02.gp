M=26;

\\ a(n) = (1/4) * Sum_{k=0..floor(n/3)} (n-2*k+2) * 2^(n-2*k) * binomial(2*n-4*k+2,2*k+1).
b(n) = (1/4) * sum(k=0, n\3, (n-2*k+2) * 2^(n-2*k) * binomial(2*n-4*k+2, 2*k+1));
for(n=0, M, print1(b(n),", "));
\\ for(n=0, M, write("/Users/xxx/Desktop/b391834_1.txt", n, " ", a(n)));

\\ a(n) = 8*a(n-1) - 24*a(n-2) + 40*a(n-3) - 32*a(n-4) - 32*a(n-5) + 40*a(n-6) - 32*a(n-7) - 96*a(n-8) + 32*a(n-9) + 64*a(n-10) - 16*a(n-12).
a(n) = if(n<12, b(n), 8*a(n-1) - 24*a(n-2) + 40*a(n-3) - 32*a(n-4) - 32*a(n-5) + 40*a(n-6) - 32*a(n-7) - 96*a(n-8) + 32*a(n-9) + 64*a(n-10) - 16*a(n-12));
for(n=0, 25, print1(a(n)-b(n),", "));
