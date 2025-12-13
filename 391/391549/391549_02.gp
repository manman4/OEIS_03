M=17;

\\ a(n) = 3 * n! * exp(-1) * Sum_{k>=0} binomial(4*n+3*k+3,n)/((4*n+3*k+3) * k!) for n > 0.
a(n) = 3 * n! * exp(-1) * sum(k=0, 100, binomial(4*n+3*k+3,n)/((4*n+3*k+3) * k!) );
for(n=0, M, print1(a(n), ", "));
a(n) = if(n==0, 1, 3 * n! * exp(-1) * sum(k=0, 100, binomial(4*n+3*k+3,n)/((4*n+3*k+3) * k!) ) );
for(n=0, M, print1(round(a(n)), ", "));

