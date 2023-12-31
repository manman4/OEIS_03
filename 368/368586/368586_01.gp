my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace( x * (1+3*x/2+x^2/2+x^3/24) * exp(x) / (1+x) )))

my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(x*sum(k=0, 3, binomial(3, k)*x^k/(k+1)!)*exp(x)/(1+x))))
