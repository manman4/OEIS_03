\\ E.g.f.: sinh(x)^7/7! = Sum_{k>=0} a(k)*x^(2*k+7)/(2*k+7)!.
a(n) = (2*n+7)! * polcoef(sinh(x + x*O(x^(2*n+7)))^7/7!, 2*n+7);
for(n=0, 15, print1(a(n), ", "));
