\\ a(n) = (1/2) * Sum_{k=0..floor(n/3)} 3^(n-2*k) * binomial(2*n-4*k+2,2*k+1).
b(n) = (1/2) * sum(k=0, n\3, 3^(n-2*k) * binomial(2*n-4*k+2,2*k+1));
for(n=0, 24, print1(b(n),", "));

\\ a(n) = 6*a(n-1) - 9*a(n-2) + 6*a(n-3) + 18*a(n-4) - 9*a(n-6).
a(n) = if(n<6, b(n), 6*a(n-1) - 9*a(n-2) + 6*a(n-3) + 18*a(n-4) - 9*a(n-6));
for(n=0, 30, print1(a(n)-b(n),", "));
