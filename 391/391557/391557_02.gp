M=17;

\\ a(n) = n! * exp(-1/3) * Sum_{k>=0} binomial(3*n+3*k+3,n)/((3*n+3*k+3) * 3^k * k!) for n > 0.
a(n) = n! * exp(-1/3) * sum(k=0, 100, binomial(3*n+3*k+3,n)/((3*n+3*k+3) * 3^k * k!) );
for(n=0, M, print1(a(n), ", "));
a(n) = if(n==0, 1, n! * exp(-1/3) * sum(k=0, 100, binomial(3*n+3*k+3,n)/((3*n+3*k+3) * 3^k * k!) ) );
for(n=0, M, print1(round(a(n)), ", "));

