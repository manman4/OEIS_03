\\ G.f.: (g-1)/(5-4*g)^2 where g=1+x*g^5.

my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(5*k, k)/(4*k+1)*x^k)); 1+x*g^5 - g
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(5*k, k)/(4*k+1)*x^k)); concat(0, Vec((g-1)/(5-4*g)^2))
my(N=30, x='x+O('x^N), g=sum(k=0, N, binomial(5*k, k)/(4*k+1)*x^k)); (g-1)/(5-4*g)^2