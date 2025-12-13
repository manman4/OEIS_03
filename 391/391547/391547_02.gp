M=17;

\\ a(n) = 4 * n! * exp(-1) * Sum_{k>=0} binomial(3*n+4*k+4,n)/((3*n+4*k+4) * k!) for n > 0.
a(n) = 4 * n! * exp(-1) * sum(k=0, 100, binomial(3*n+4*k+4,n)/((3*n+4*k+4) * k!) );
for(n=0, M, print1(a(n), ", "));
a(n) = if(n==0, 1, 4 * n! * exp(-1) * sum(k=0, 100, binomial(3*n+4*k+4,n)/((3*n+4*k+4) * k!) ) );
for(n=0, M, print1(round(a(n)), ", "));

