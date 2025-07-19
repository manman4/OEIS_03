\\ G.f.: g*(1-g)/(1-5*g)^2 where g*(1-g)^4 = x.

my(N=30, x='x+O('x^N), g=x*sum(k=0, N, binomial(5*k+3, k)/(k+1)*x^k)); concat(0, Vec(g*(1-g)^4)) 
my(N=30, x='x+O('x^N), g=x*sum(k=0, N, binomial(5*k+3, k)/(k+1)*x^k)); concat(0, Vec(g*(1-g)/(1-5*g)^2)) 