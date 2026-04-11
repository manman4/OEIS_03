\\ a(0) = 1; a(n) = Sum_{k=0..n-1} (-1)^(n-k-1) * binomial(n,k) * (k+3)^(3*(n-k)) * a(k).
a(n) = if(n==0, 1, sum(k=0, n-1, (-1)^(n-k-1) * binomial(n,k) * (k+3)^(3*(n-k)) * a(k)));
for(n=0, 12, print1(a(n),", "));

print();

\\ log(1+27*x) = Sum_{k>=1} a(k)/k * (x/(1 + (k+3)^3*x))^k.
my(N=15, x='x+O('x^N)); print(sum(k=1, N, a(k)/k * (x/(1+(k+3)^3*x))^k) - log(1+27*x));

print();

\\ 1 = Sum_{k>=0} a(k) * binomial(k+m-1,k) * x^k/(1 + (k+3)^3*x)^(k+m) for m >= 1.
for(m=1, 5, my(N=15, x='x+O('x^N)); print(sum(k=0, N, a(k) * binomial(k+m-1,k) * x^k/(1+(k+3)^3*x)^(k+m))));

print();

\\ 1 = Sum_{k>=0} a(k) * x^k/k! * exp(-(k+3)^3*x).
my(N=15, x='x+O('x^N)); print(sum(k=0, N, a(k) * x^k/k! * exp(-(k+3)^3*x)));

