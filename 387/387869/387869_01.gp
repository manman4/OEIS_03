\\ a(n) = Sum_{k=0..n} binomial(2*n+1,4*k).
b(n) = sum(k=0, n, binomial(2*n+1, 4*k));
for(n=0, 25, print1(b(n),", "));

\\ a(n) = 4*a(n-1) - 4*a(n-2) + 16*a(n-3) for n > 2.
a(n) = if(n<3, b(n), 4*a(n-1) - 4*a(n-2) + 16*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));

\\ a(2*n) = A090407(n), a(2*n+1) = A090408(n).
for(n=0, 21, print1(b(2*n),", "));
for(n=0, 21, print1(b(2*n+1),", "));
