\\ Sum_{k=0..n} a(k) * a(n-k) = A169961(n).
a(n) = my(x='x+O('x^(n+3)), g=sum(k=0, n, binomial(12*k, k)/(11*k+1)*x^k)); polcoef(1/sqrt(1 - 12*x*g^11), n);
for(n=0, 50, print1(sum(k=0, n, a(k) * a(n-k)) - binomial(12*n, n),", "));

\\ G.f.: 1/sqrt(1 - 12*x*g^11) where g = 1+x*g^12.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(12*k, k)/(11*k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(12*k, k)/(11*k+1)*x^k)); Vec( 1+x*g^12 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(12*k, k)/(11*k+1)*x^k)); Vec( 1/sqrt(1 - 12*x*g^11) )