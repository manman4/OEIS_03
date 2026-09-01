\\ G.f.: Sum_{n>=0} Sum_{k=0..floor(n/2)} T(n,k)*x^n*y^k = Sum_{j>=0} j! * ( ((1-y)*(2*x^2-x^3)-x)/((1-y)*x^2-1) )^j.

T(n, k) = polcoef(polcoef(sum(j=0, n, j!*(((1-y)*(2*x^2-x^3)-x)/((1-y)*x^2-1+O(x^(n+1))))^j), n), k);
for(n=0, 11, for(k=0, n\2, print1(T(n,k),", ")));

