\\ G.f.: ( 1 + sqrt(1-4*x) - 2*sqrt( (1 + sqrt(1-4*x) - 18*x)/2 ) )/8.
my(N=30, x='x+O('x^N)); (1+sqrt(1-4*x)-2*sqrt((1+sqrt(1-4*x)-18*x)/2))/8
my(N=30, x='x+O('x^N)); concat(0, Vec((1+sqrt(1-4*x)-2*sqrt((1+sqrt(1-4*x)-18*x)/2))/8))