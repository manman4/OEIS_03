\\ G.f.: (1-2*x-x^2)/((1-4*x) * (1+x+x^2)).
f = (1-2*x-x^2)/((1-4*x) * (1+x+x^2));
gcd(numerator(f), denominator(f))
my(N=30, x='x+O('x^N)); Vec((1-2*x-x^2)/((1-4*x) * (1+x+x^2)))
