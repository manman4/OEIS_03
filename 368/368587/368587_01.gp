my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace( x * (1+2*x+x^2+x^3/6+x^4/120) * exp(x) / (1+x) )))

my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(x*sum(k=0, 4, binomial(4, k)*x^k/(k+1)!)*exp(x)/(1+x))))
