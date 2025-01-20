\\ a(n) = e * (-2)^n * n! * Sum_{k>=0} (-1)^k * binomial(-k/2,n)/k!.
a(n) = round(exp(1) * (-2)^n * n! * sum(k=0, 1000, (-1)^k * binomial(-k/2, n)/k!));
for(n=0, 20, print1(a(n), ", "))
