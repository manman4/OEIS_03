M=17;

\\ a(n) = 2 * n! * exp(-1) * Sum_{k>=0} binomial(2*n+4*k+4,n)/((n+2*k+2) * k!) for n > 0.
a(n) = 2 * n! * exp(-1) * sum(k=0, 100, binomial(2*n+4*k+4,n)/((n+2*k+2) * k!) );
for(n=0, M, print1(a(n), ", "));
a(n) = if(n==0, 1, 2 * n! * exp(-1) * sum(k=0, 100, binomial(2*n+4*k+4,n)/((n+2*k+2) * k!) ) );
for(n=0, M, print1(round(a(n)), ", "));

