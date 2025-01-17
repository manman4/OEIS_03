\\ a(n) = (1/e) * (-2)^n * n! * Sum_{k>=0} binomial(-3*k/2,n)/k!.
a(n) = round( 1/exp(1) * (-2)^n * n! * sum(k=0, 1000, binomial(-3*k/2, n)/k!) );
for(n=0, 20, print1(a(n), ", "))
