M=19;

\\ a(n) = (10*n-11) * a(n-1) - (5*n-8)^2 * a(n-2) for n > 1.
a(n) = if(n<2, n, (10*n-11) * a(n-1) - (5*n-8)^2 * a(n-2));
for(n=0, M, print1(a(n), ", "));

