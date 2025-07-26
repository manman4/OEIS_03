\\ G.f.: g/(1-6*g)^2 where g*(1-g)^5 = x.

my(N=30, x='x+O('x^N), g=x*sum(k=0, N, binomial(6*k+4, k)/(k+1)*x^k)); concat(0, Vec(g*(1-g)^5)) 
my(N=30, x='x+O('x^N), g=x*sum(k=0, N, binomial(6*k+4, k)/(k+1)*x^k)); concat(0, Vec(g/(1-6*g)^2)) 
my(N=30, x='x+O('x^N), g=x*sum(k=0, N, binomial(6*k+4, k)/(k+1)*x^k)); g/(1-6*g)^2