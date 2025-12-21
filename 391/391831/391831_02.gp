M=32;

\\ a(n) = (1/4) * Sum_{k=0..floor(n/3)} (n-2*k+2) * binomial(2*n-4*k+2,2*k+1).
b(n) = (1/4) * sum(k=0, n\3, (n-2*k+2) * binomial(2*n-4*k+2, 2*k+1));
for(n=0, M, print1(b(n),", "));
\\ for(n=0, M, write("/Users/xxx/Desktop/b391831_1.txt", n, " ", a(n)));

\\ a(n) = 4*a(n-1) - 6*a(n-2) + 8*a(n-3) - 5*a(n-4) - 4*a(n-5) - 2*a(n-6) - 4*a(n-7) - 6*a(n-8) + 4*a(n-9) + 4*a(n-10) - a(n-12).
a(n) = if(n<12, b(n), 4*a(n-1) - 6*a(n-2) + 8*a(n-3) - 5*a(n-4) - 4*a(n-5) - 2*a(n-6) - 4*a(n-7) - 6*a(n-8) + 4*a(n-9) + 4*a(n-10) - a(n-12));
for(n=0, 25, print1(a(n)-b(n),", "));