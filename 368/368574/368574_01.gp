my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace( x * (1+x+x^2/6) * exp(x) / (1-x) )))

my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(x*sum(k=0, 2, binomial(2, k)*x^k/(k+1)!)*exp(x)/(1-x))))
