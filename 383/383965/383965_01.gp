\\ Sum_{k=0..n} a(k) * a(n-k) = A004381(n).
a(n) = my(x='x+O('x^(n+3)), g=sum(k=0, n, binomial(8*k, k)/(7*k+1)*x^k)); polcoef(1/sqrt(1 - 8*x*g^7), n);
for(n=0, 50, print1(sum(k=0, n, a(k) * a(n-k)) - binomial(8*n, n),", "));

\\ G.f.: 1/sqrt(1 - 8*x*g^7) where g = 1+x*g^8 is the g.f. of A007556.
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(8*k, k)/(7*k+1)*x^k)); Vec( g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(8*k, k)/(7*k+1)*x^k)); Vec( 1+x*g^8 - g )
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(8*k, k)/(7*k+1)*x^k)); Vec( 1/sqrt(1 - 8*x*g^7) )