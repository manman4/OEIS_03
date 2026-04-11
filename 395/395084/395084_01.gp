\\ a(0) = 1; a(n) = Sum_{k=0..n-1} (-1)^(n-k-1) * binomial(n,k) * (k+3)^(2*(n-k)) * a(k).
a(n) = if(n==0, 1, sum(k=0, n-1, (-1)^(n-k-1) * binomial(n,k) * (k+3)^(2*(n-k)) * a(k)));
for(n=0, 14, print1(a(n),", "));

print();

\\ log(1+9*x) = Sum_{k>=1} a(k)/k * (x/(1 + (k+3)^2*x))^k.
for(k=1, 5, my(N=15, x='x+O('x^N)); print(sum(j=1, N, a(j)/j * (x/(1+(j+3)^2*x))^j) - log(1+9*x)));

print();

\\ 1 = Sum_{k>=0} a(k) * binomial(k+m-1,k) * x^k/(1 + (k+3)^2*x)^(k+m) for m >= 1.
for(k=1, 5, for(m=1, 5, my(N=15, x='x+O('x^N)); print(sum(j=0, N, a(j) * binomial(j+m-1,j) * x^j/(1+(j+3)^2*x)^(j+m)))));

print();

\\ 1 = Sum_{k>=0} a(k) * x^k/k! * exp(-(k+3)^2*x).
for(k=1, 5, my(N=15, x='x+O('x^N)); print(sum(j=0, N, a(j) * x^j/j! * exp(-(j+3)^2*x))));

