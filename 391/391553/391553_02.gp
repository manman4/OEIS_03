M=18;

\\ a(n) = n! * exp(-1/2) * Sum_{k>=0} binomial(2*n+2*k+2,n)/((2*n+2*k+2) * 2^k * k!) for n > 0.
a(n) = n! * exp(-1/2) * sum(k=0, 100, binomial(2*n+2*k+2,n)/((2*n+2*k+2) * 2^k * k!) );
for(n=0, M, print1(a(n), ", "));
a(n) = if(n==0, 1, n! * exp(-1/2) * sum(k=0, 100, binomial(2*n+2*k+2,n)/((2*n+2*k+2) * 2^k * k!) ) );
for(n=0, M, print1(round(a(n)), ", "));

