\\ E.g.f.: sinh(x)^8/8! = Sum_{k>=0} 4^k * a(k) * x^(2*k+8)/(2*k+8)!.
my(N=40, x='x+O('x^N)); Vec(serlaplace(sinh(x)^8/8!)) 
a(n) = my(x='x+O('x^(2*n+9))); (2*n+8)!/4^n * polcoef(sinh(x)^8/8!, 2*n+8); 
for(n=0, 20, print1(a(n),", "));

