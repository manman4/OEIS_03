M=17;

\\ a(n) = n! * exp(-1/4) * Sum_{k>=0} binomial(4*n+4*k+4,n)/((4*n+4*k+4) * 4^k * k!) for n > 0.
a(n) = n! * exp(-1/4) * sum(k=0, 100, binomial(4*n+4*k+4,n)/((4*n+4*k+4) * 4^k * k!) );
for(n=0, M, print1(a(n), ", "));
a(n) = if(n==0, 1, n! * exp(-1/4) * sum(k=0, 100, binomial(4*n+4*k+4,n)/((4*n+4*k+4) * 4^k * k!) ) );
for(n=0, M, print1(round(a(n)), ", "));

