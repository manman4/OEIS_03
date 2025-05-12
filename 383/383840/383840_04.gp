\\ E.g.f.: sinh(x)^10/10! = Sum_{k>=0} 4^k * a(k) * x^(2*k+10)/(2*k+10)!.
my(N=40, x='x+O('x^N)); Vec(serlaplace(sinh(x)^10/10!))
a(n) = my(x='x+O('x^(2*n+11))); (2*n+10)!/4^n * polcoef(sinh(x)^10/10!, 2*n+10);
for(n=0, 20, print1(a(n),", "));

