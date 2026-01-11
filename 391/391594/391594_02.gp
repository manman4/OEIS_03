\\ a(n) = Sum_{k=0..2*n} binomial(2*k,2*n-k).
b(n) = sum(k=0, 2*n, binomial(2*k, 2*n-k));
for(n=0, 24, print1(b(n), ", "));

\\ a(n) = 5*a(n-1) - 2*a(n-2) + a(n-3).
a(n) = if(n<3, b(n), 5*a(n-1) - 2*a(n-2) + a(n-3));
for(n=0, 20, print1(a(n)-b(n), ", "));

a00930(n) = ([0, 1, 0; 0, 0, 1; 1, 0, 1]^n*[1; 1; 1])[1, 1];
for(n=0, 20, print1(a00930(n), ", "));
\\ a(n) = A000930(4*n).
for(n=0, 100, print1(b(n)-a00930(4*n), ", "));