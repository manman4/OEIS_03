\\ L.g.f.: Sum_{k>=1} a(k)*x^k/k = (1/3) * log( Sum_{k>=0} binomial(4*k-1,k)*x^k ).

a(n) = my(x='x+O('x^(n+1))); n * polcoef(1/3 * log(sum(k=0, n, binomial(4*k-1, k)*x^k)), n);
for(n=0, 21, print1(a(n), ", ");)
